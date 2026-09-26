#include "AutocompleteEngine.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

namespace {
constexpr int kPort = 8080;
constexpr int kTopK = 5;

std::string readFile(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file) return {};
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
}

std::string urlDecode(const std::string& value) {
    std::string decoded;
    decoded.reserve(value.size());
    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '+' ) decoded += ' ';
        else if (value[i] == '%' && i + 2 < value.size()) {
            const auto hex = value.substr(i + 1, 2);
            try { decoded += static_cast<char>(std::stoi(hex, nullptr, 16)); i += 2; }
            catch (const std::exception&) { decoded += '%'; }
        } else decoded += value[i];
    }
    return decoded;
}

std::string queryValue(const std::string& query, const std::string& key) {
    std::istringstream stream(query);
    std::string part;
    while (std::getline(stream, part, '&')) {
        const auto separator = part.find('=');
        if (separator != std::string::npos && part.substr(0, separator) == key)
            return urlDecode(part.substr(separator + 1));
    }
    return {};
}

std::string escapeJson(const std::string& value) {
    std::string result;
    for (char character : value) {
        if (character == '"' || character == '\\') result += '\\';
        result += character;
    }
    return result;
}

void sendResponse(SOCKET client, const std::string& status, const std::string& type,
                  const std::string& body) {
    const std::string response = "HTTP/1.1 " + status + "\r\nContent-Type: " + type +
        "; charset=utf-8\r\nContent-Length: " + std::to_string(body.size()) +
        "\r\nConnection: close\r\n\r\n" + body;
    send(client, response.c_str(), static_cast<int>(response.size()), 0);
}

void handleClient(SOCKET client, autocomplete::AutocompleteEngine& engine) {
    char buffer[8192]{};
    const int received = recv(client, buffer, sizeof(buffer) - 1, 0);
    if (received <= 0) return;
    const std::string request(buffer, static_cast<std::size_t>(received));
    const auto firstLineEnd = request.find("\r\n");
    if (firstLineEnd == std::string::npos) return;
    std::istringstream line(request.substr(0, firstLineEnd));
    std::string method, target, version;
    line >> method >> target >> version;
    const auto queryStart = target.find('?');
    const std::string path = target.substr(0, queryStart);
    const std::string query = queryStart == std::string::npos ? "" : target.substr(queryStart + 1);

    if (method == "GET" && path == "/api/autocomplete") {
        const std::string prefix = queryValue(query, "prefix");
        const auto results = engine.autocomplete(prefix, kTopK);
        std::string body = "{\"suggestions\":[";
        for (std::size_t i = 0; i < results.size(); ++i) {
            if (i) body += ',';
            body += '"' + escapeJson(results[i]) + '"';
        }
        sendResponse(client, "200 OK", "application/json", body + "]}");
    } else if (method == "POST" && path == "/api/select") {
        const std::string word = queryValue(query, "word");
        engine.recordUsage(word);
        const bool saved = engine.saveData("data/usage.txt");
        sendResponse(client, saved ? "200 OK" : "500 Internal Server Error", "application/json",
                     saved ? "{\"ok\":true}" : "{\"ok\":false,\"error\":\"Could not save usage\"}");
    } else if (method == "GET" && (path == "/" || path == "/index.html")) {
        sendResponse(client, "200 OK", "text/html", readFile("web/index.html"));
    } else if (method == "GET" && path == "/styles.css") {
        sendResponse(client, "200 OK", "text/css", readFile("web/styles.css"));
    } else if (method == "GET" && path == "/app.js") {
        sendResponse(client, "200 OK", "application/javascript", readFile("web/app.js"));
    } else {
        sendResponse(client, "404 Not Found", "text/plain", "Not found");
    }
}
}  // namespace

int main() {
    autocomplete::AutocompleteEngine engine;
    if (!engine.loadData("data/words.txt", "data/usage.txt")) {
        std::cerr << "Unable to load data/words.txt. Run from the project directory.\n";
        return 1;
    }
    WSADATA winsockData{};
    if (WSAStartup(MAKEWORD(2, 2), &winsockData) != 0) return 1;
    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(kPort);
    if (server == INVALID_SOCKET || bind(server, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR ||
        listen(server, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Could not start server on http://127.0.0.1:" << kPort << "\n";
        if (server != INVALID_SOCKET) closesocket(server);
        WSACleanup();
        return 1;
    }
    std::cout << "Autocomplete web app running at http://127.0.0.1:" << kPort << "\nPress Ctrl+C to stop.\n";
    while (true) {
        const SOCKET client = accept(server, nullptr, nullptr);
        if (client != INVALID_SOCKET) { handleClient(client, engine); closesocket(client); }
    }
}
