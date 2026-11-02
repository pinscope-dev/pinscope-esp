#pragma once

// The page: it asks the device for /data.json every five seconds and draws it.
const char PAGE[] PROGMEM = R"html(<!doctype html>
<html lang="en"><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Sensor</title>
<style>
  body { font: 16px system-ui, sans-serif; max-width: 32rem; margin: 2rem auto; padding: 0 1rem; color: #222; background: #fafafa; }
  @media (prefers-color-scheme: dark) { body { color: #eee; background: #161616; } }
  #value { font-size: 4rem; font-weight: 600; margin: .5rem 0; }
  svg { width: 100%; height: 6rem; }
  .muted { opacity: .6; font-size: .9rem; }
</style>
<h1 id="name">Sensor</h1>
<div id="value">…</div>
<svg viewBox="0 0 59 20" preserveAspectRatio="none"><polyline id="line" fill="none" stroke="currentColor" vector-effect="non-scaling-stroke" stroke-width="2"/></svg>
<p class="muted" id="note">Last ten minutes</p>
<script>
async function update() {
  try {
    const d = await (await fetch('data.json')).json();
    document.getElementById('name').textContent = d.name;
    document.getElementById('value').textContent = d.value.toFixed(1) + ' ' + d.unit;
    const lo = Math.min(...d.history), hi = Math.max(...d.history), span = hi - lo;
    document.getElementById('line').setAttribute('points',
      d.history.map((v, i) => (i + 60 - d.history.length) + ',' + (span ? 19 - (v - lo) / span * 18 : 10)).join(' '));
    document.getElementById('note').textContent =
      'Last ten minutes: ' + lo.toFixed(1) + ' to ' + hi.toFixed(1) + ' ' + d.unit;
  } catch (e) {
    document.getElementById('note').textContent = 'The device is not answering right now.';
  }
}
update();
setInterval(update, 5000);
</script>
)html";
