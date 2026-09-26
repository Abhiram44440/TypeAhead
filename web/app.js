const input = document.querySelector('#prefix');
const list = document.querySelector('#suggestions');
const hint = document.querySelector('#hint');
const empty = document.querySelector('#empty');
const toast = document.querySelector('#toast');
let words = [];
let activeIndex = -1;
let timer;

function highlight(word, prefix) {
  const safe = word.replaceAll('&', '&amp;').replaceAll('<', '&lt;').replaceAll('>', '&gt;');
  return `<mark>${safe.slice(0, prefix.length)}</mark>${safe.slice(prefix.length)}`;
}
function render() {
  const prefix = input.value.trim().toLowerCase();
  list.innerHTML = words.map((word, index) => `<li><button class="suggestion ${index === activeIndex ? 'active' : ''}" data-index="${index}"><span class="number">0${index + 1}</span><span class="word">${highlight(word, prefix)}</span><span class="select-label">SELECT ↵</span></button></li>`).join('');
  empty.hidden = words.length > 0 || !prefix;
  hint.textContent = prefix ? `${words.length} suggestion${words.length === 1 ? '' : 's'} found` : 'Enter at least one letter to see suggestions.';
  document.querySelectorAll('.suggestion').forEach(button => button.addEventListener('click', () => selectWord(Number(button.dataset.index))));
}
async function search() {
  const prefix = input.value.trim();
  if (!prefix) { words = []; activeIndex = -1; render(); return; }
  const response = await fetch(`/api/autocomplete?prefix=${encodeURIComponent(prefix)}`);
  words = (await response.json()).suggestions;
  activeIndex = words.length ? 0 : -1;
  render();
}
async function selectWord(index) {
  const word = words[index];
  if (!word) return;
  const response = await fetch(`/api/select?word=${encodeURIComponent(word)}`, { method: 'POST' });
  if (!response.ok) { toast.textContent = 'Could not save your selection.'; toast.classList.add('show'); return; }
  // Clear the input after successful selection
    input.value = '';

    // Clear suggestions
    words = [];
    activeIndex = -1;
  toast.textContent = `Saved “${word}” — future results will learn from it.`;
  toast.classList.add('show'); setTimeout(() => toast.classList.remove('show'), 2600);
  search();
}
input.addEventListener('input', () => { clearTimeout(timer); timer = setTimeout(search, 130); });
input.addEventListener('keydown', event => {
  if (event.key === 'ArrowDown' && words.length) { event.preventDefault(); activeIndex = (activeIndex + 1) % words.length; render(); }
  if (event.key === 'ArrowUp' && words.length) { event.preventDefault(); activeIndex = (activeIndex - 1 + words.length) % words.length; render(); }
  if (event.key === 'Enter' && activeIndex >= 0) { event.preventDefault(); selectWord(activeIndex); }
  if (event.key === 'Escape') { input.value = ''; words = []; activeIndex = -1; render(); }
});
