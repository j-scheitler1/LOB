const snapshots = window.snapshots || [];

let index = 0;
let timer = null;

const formatter = new Intl.NumberFormat(undefined, { maximumFractionDigits: 2 });

const playButton = document.getElementById('play');
const prevButton = document.getElementById('prev');
const nextButton = document.getElementById('next');
const scrubber = document.getElementById('scrubber');
const eventTime = document.getElementById('eventTime');
const stockPrice = document.getElementById('stockPrice');
const bestBid = document.getElementById('bestBid');
const bestAsk = document.getElementById('bestAsk');
const lastOrder = document.getElementById('lastOrder');
const bidsTable = document.getElementById('bids');
const asksTable = document.getElementById('asks');
const priceChart = document.getElementById('priceChart');

scrubber.max = Math.max(snapshots.length - 1, 0);

function fmt(value) {
  if (value === null || value === undefined) {
    return 'none';
  }
  return formatter.format(value);
}

function scale(value, oldMin, oldMax, newMin, newMax) {
  if (oldMin === oldMax) {
    return (newMin + newMax) / 2;
  }
  return newMin + ((value - oldMin) / (oldMax - oldMin)) * (newMax - newMin);
}

function priceRange() {
  const prices = snapshots
    .map(snapshot => snapshot.stock_price)
    .filter(price => typeof price === 'number');

  if (prices.length === 0) {
    return [0, 1];
  }

  let low = Math.min(...prices);
  let high = Math.max(...prices);

  if (low === high) {
    low -= 1;
    high += 1;
  }

  return [low, high];
}

const [minPrice, maxPrice] = priceRange();

function drawPriceChart() {
  const ctx = priceChart.getContext('2d');
  const margin = 35;
  const width = priceChart.width;
  const height = priceChart.height;

  ctx.clearRect(0, 0, width, height);

  if (snapshots.length === 0) {
    return;
  }

  ctx.strokeStyle = '#999';
  ctx.beginPath();
  ctx.moveTo(margin, margin);
  ctx.lineTo(margin, height - margin);
  ctx.lineTo(width - margin, height - margin);
  ctx.stroke();

  ctx.fillStyle = '#333';
  ctx.font = '12px Arial';
  ctx.fillText(fmt(maxPrice), 4, margin + 4);
  ctx.fillText(fmt(minPrice), 4, height - margin + 4);
  ctx.fillText('event ' + snapshots[index].event_time, width - 95, height - 12);

  ctx.strokeStyle = '#2563eb';
  ctx.beginPath();

  snapshots.slice(0, index + 1).forEach((snapshot, pointIndex) => {
    const x = scale(pointIndex, 0, snapshots.length - 1, margin, width - margin);
    const y = scale(snapshot.stock_price, minPrice, maxPrice, height - margin, margin);

    if (pointIndex === 0) {
      ctx.moveTo(x, y);
    } else {
      ctx.lineTo(x, y);
    }
  });

  ctx.stroke();

  const current = snapshots[index];
  const currentX = scale(index, 0, snapshots.length - 1, margin, width - margin);
  const currentY = scale(current.stock_price, minPrice, maxPrice, height - margin, margin);

  ctx.fillStyle = '#dc2626';
  ctx.beginPath();
  ctx.arc(currentX, currentY, 4, 0, Math.PI * 2);
  ctx.fill();
}

function showLevels(tableBody, levels) {
  tableBody.innerHTML = '';

  if (levels.length === 0) {
    tableBody.innerHTML = '<tr><td colspan="2">none</td></tr>';
    return;
  }

  levels.forEach(level => {
    const row = document.createElement('tr');
    const price = document.createElement('td');
    const volume = document.createElement('td');

    price.textContent = fmt(level.price);
    volume.textContent = fmt(level.volume);

    row.appendChild(price);
    row.appendChild(volume);
    tableBody.appendChild(row);
  });
}

function showEmptyPage() {
  eventTime.textContent = 'no data';
  stockPrice.textContent = 'none';
  bestBid.textContent = 'none';
  bestAsk.textContent = 'none';
  lastOrder.textContent = 'none';
  showLevels(bidsTable, []);
  showLevels(asksTable, []);
  drawPriceChart();
}

function render() {
  if (snapshots.length === 0) {
    showEmptyPage();
    return;
  }

  const snapshot = snapshots[index];

  scrubber.value = index;
  eventTime.textContent = snapshot.event_time;
  stockPrice.textContent = fmt(snapshot.stock_price);
  bestBid.textContent = fmt(snapshot.best_bid);
  bestAsk.textContent = fmt(snapshot.best_ask);
  lastOrder.textContent = (snapshot.side || 'none') + ' T' + (snapshot.trader_id ?? '?') + ' @ ' + fmt(snapshot.limit_price);

  showLevels(bidsTable, snapshot.bids);
  showLevels(asksTable, snapshot.asks);
  drawPriceChart();
}

function step(amount) {
  if (snapshots.length === 0) {
    return;
  }

  index = (index + amount + snapshots.length) % snapshots.length;
  render();
}

function playOrPause() {
  if (snapshots.length === 0) {
    return;
  }

  if (timer) {
    clearInterval(timer);
    timer = null;
    playButton.textContent = 'Play';
  } else {
    timer = setInterval(() => step(1), 450);
    playButton.textContent = 'Pause';
  }
}

playButton.addEventListener('click', playOrPause);
prevButton.addEventListener('click', () => step(-1));
nextButton.addEventListener('click', () => step(1));

scrubber.addEventListener('input', () => {
  index = Number(scrubber.value);
  render();
});

render();
