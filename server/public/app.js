const elements = {
  health: document.querySelector("#health"),
  payloadCount: document.querySelector("#payloadCount"),
  apiCount: document.querySelector("#apiCount"),
  serverCount: document.querySelector("#serverCount"),
  activity: document.querySelector("#activity"),
  refresh: document.querySelector("#refresh"),
  autoRefresh: document.querySelector("#autoRefresh"),
  requestSearch: document.querySelector("#requestSearch"),
  methodFilter: document.querySelector("#methodFilter"),
  statusFilter: document.querySelector("#statusFilter"),
  requestSort: document.querySelector("#requestSort"),
  requestLimit: document.querySelector("#requestLimit"),
  requestSummary: document.querySelector("#requestSummary"),
  payloadModal: document.querySelector("#payloadModal"),
  payloadModalTitle: document.querySelector("#payloadModalTitle"),
  payloadMeta: document.querySelector("#payloadMeta"),
  payloadDetails: document.querySelector("#payloadDetails"),
  closePayloadModal: document.querySelector("#closePayloadModal"),
  columnToggles: [...document.querySelectorAll("[data-column-toggle]")],
};

const columnPreferenceKey = "arduino-playground-visible-columns";
const autoRefreshPreferenceKey = "arduino-playground-auto-refresh";
let requests = [];

function escapeHtml(value) {
  return String(value).replace(/[&<>'"]/g, (character) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", "'": "&#39;", '"': "&quot;",
  })[character]);
}

function readColumnPreferences() {
  try {
    return JSON.parse(localStorage.getItem(columnPreferenceKey)) ?? {};
  } catch {
    return {};
  }
}

function applyColumnPreferences() {
  const preferences = readColumnPreferences();
  for (const toggle of elements.columnToggles) {
    const column = toggle.dataset.columnToggle;
    if (column in preferences) toggle.checked = preferences[column];
    document.querySelectorAll(`[data-column="${column}"]`).forEach((cell) => {
      cell.hidden = !toggle.checked;
    });
  }
}

function saveColumnPreferences() {
  const preferences = Object.fromEntries(
    elements.columnToggles.map((toggle) => [toggle.dataset.columnToggle, toggle.checked]),
  );
  localStorage.setItem(columnPreferenceKey, JSON.stringify(preferences));
  applyColumnPreferences();
}

function updateMethodFilter() {
  const selected = elements.methodFilter.value;
  const methods = [...new Set(requests.map((event) => event.detail.method).filter(Boolean))].sort();
  elements.methodFilter.innerHTML = '<option value="all">All</option>'
    + methods.map((method) => `<option value="${escapeHtml(method)}">${escapeHtml(method)}</option>`).join("");
  elements.methodFilter.value = methods.includes(selected) ? selected : "all";
}

function filteredRequests() {
  const query = elements.requestSearch.value.trim().toLocaleLowerCase();
  const method = elements.methodFilter.value;
  const statusClass = elements.statusFilter.value;
  const sort = elements.requestSort.value;

  return requests.filter((event) => {
    const detail = event.detail;
    if (method !== "all" && detail.method !== method) return false;
    if (statusClass !== "all" && Math.floor((detail.status ?? 0) / 100) !== Number(statusClass)) return false;
    return !query || JSON.stringify(event).toLocaleLowerCase().includes(query);
  }).sort((left, right) => {
    if (sort === "oldest") return new Date(left.timestamp) - new Date(right.timestamp);
    if (sort === "status-asc") return (left.detail.status ?? 0) - (right.detail.status ?? 0);
    if (sort === "status-desc") return (right.detail.status ?? 0) - (left.detail.status ?? 0);
    return new Date(right.timestamp) - new Date(left.timestamp);
  });
}

function openPayloadModal(event) {
  const detail = event.detail;
  if (!detail.payload) return;
  elements.payloadModalTitle.textContent = `${detail.method} ${detail.path}`;
  elements.payloadMeta.innerHTML = `
    <div><dt>Time</dt><dd>${escapeHtml(new Date(event.timestamp).toLocaleString("en-GB"))}</dd></div>
    <div><dt>Device</dt><dd>${escapeHtml(detail.deviceId ?? "—")}</dd></div>
    <div><dt>Status</dt><dd>${escapeHtml(detail.status ?? "—")}</dd></div>`;
  elements.payloadDetails.textContent = JSON.stringify(detail.payload, null, 2);
  elements.payloadModal.showModal();
}

function renderRequests() {
  const matching = filteredRequests();
  const limit = Number(elements.requestLimit.value);
  const visible = matching.slice(0, limit);

  elements.activity.innerHTML = visible.map((event) => {
    const detail = event.detail;
    const hasPayload = Boolean(detail.payload);
    return `
      <tr data-request-id="${escapeHtml(event.id)}" class="${hasPayload ? "payload-request" : ""}" ${hasPayload ? 'tabindex="0"' : ""}>
        <td class="time">${new Date(event.timestamp).toLocaleString("en-GB")}</td>
        <td data-column="deviceId">${escapeHtml(detail.deviceId ?? "—")}</td>
        <td data-column="clientName">${escapeHtml(detail.clientName ?? "—")}</td>
        <td data-column="path"><code>${escapeHtml(detail.path ?? "—")}</code></td>
        <td data-column="method"><span class="method">${escapeHtml(detail.method ?? "—")}</span></td>
        <td data-column="status"><span class="http-status status-${Math.floor((detail.status ?? 0) / 100)}xx">${escapeHtml(detail.status ?? "—")}</span></td>
        <td>${hasPayload ? '<button type="button" class="payload-button">View</button>' : "—"}</td>
      </tr>`;
  }).join("") || '<tr><td class="empty" colspan="7">No matching requests.</td></tr>';

  elements.requestSummary.textContent = `Showing ${visible.length} of ${matching.length} matching requests (${requests.length} total).`;
  applyColumnPreferences();
}

async function refresh() {
  elements.refresh.disabled = true;
  try {
    const dashboardRequest = { headers: { "X-Monitor-Request": "dashboard" } };
    const [healthResponse, payloadResponse, activityResponse] = await Promise.all([
      fetch("/api/health", dashboardRequest),
      fetch("/api/payloads/count", dashboardRequest),
      fetch("/api/activity", dashboardRequest),
    ]);
    if (![healthResponse, payloadResponse, activityResponse].every((response) => response.ok)) {
      throw new Error("Serveren returnerede en fejl");
    }
    const [health, payloadSummary, activity] = await Promise.all([
      healthResponse.json(), payloadResponse.json(), activityResponse.json(),
    ]);
    elements.health.textContent = `API online · ${new Date(health.timestamp).toLocaleTimeString("en-GB")}`;
    elements.health.className = "status online";
    elements.payloadCount.textContent = payloadSummary.count;
    elements.apiCount.textContent = activity.filter((event) => event.source === "api").length;
    elements.serverCount.textContent = activity.filter((event) => event.source === "server").length;
    requests = activity.filter((event) => event.source === "api");
    updateMethodFilter();
    renderRequests();
  } catch {
    elements.health.textContent = "Server offline";
    elements.health.className = "status";
  } finally {
    elements.refresh.disabled = false;
  }
}

elements.refresh.addEventListener("click", refresh);
elements.columnToggles.forEach((toggle) => toggle.addEventListener("change", saveColumnPreferences));
[elements.requestSearch, elements.methodFilter, elements.statusFilter, elements.requestSort, elements.requestLimit]
  .forEach((control) => control.addEventListener("input", renderRequests));
elements.activity.addEventListener("click", (event) => {
  const row = event.target.closest("[data-request-id]");
  const request = requests.find((item) => item.id === row?.dataset.requestId);
  if (request) openPayloadModal(request);
});
elements.activity.addEventListener("keydown", (event) => {
  if (!["Enter", " "].includes(event.key)) return;
  const row = event.target.closest("[data-request-id]");
  const request = requests.find((item) => item.id === row?.dataset.requestId);
  if (request?.detail.payload) {
    event.preventDefault();
    openPayloadModal(request);
  }
});
elements.closePayloadModal.addEventListener("click", () => elements.payloadModal.close());
elements.payloadModal.addEventListener("click", (event) => {
  if (event.target === elements.payloadModal) elements.payloadModal.close();
});
elements.autoRefresh.addEventListener("change", () => {
  localStorage.setItem(autoRefreshPreferenceKey, String(elements.autoRefresh.checked));
});

elements.autoRefresh.checked = localStorage.getItem(autoRefreshPreferenceKey) !== "false";
applyColumnPreferences();
refresh();
setInterval(() => {
  if (elements.autoRefresh.checked && !document.hidden) refresh();
}, 3000);
