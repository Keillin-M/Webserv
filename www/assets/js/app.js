// Webserv Test Playground - Main JavaScript

// Utility function to display response
function displayResponse(method, url, response, body, duration) {
    const container = document.getElementById('response-container');
    if (!container) return;

    container.classList.add('show');

    // Status code color
    const statusClass = response.status >= 200 && response.status < 300 ? 'status-success' : 'status-error';

    // For error status codes (4xx, 5xx), only show basic info
    if (response.status >= 400) {
        container.innerHTML = `
            <h3>Response Details</h3>
            <div class="response-header">
                <div class="response-item">
                    <label>Method</label>
                    <div class="value">${escapeHtml(method)}</div>
                </div>
                <div class="response-item ${statusClass}">
                    <label>Status Code</label>
                    <div class="value">${response.status} ${escapeHtml(response.statusText)}</div>
                </div>
                <div class="response-item">
                    <label>Duration</label>
                    <div class="value">${duration} ms</div>
                </div>
                <div class="response-item">
                    <label>URL</label>
                    <div class="value" style="word-break: break-all;">${escapeHtml(url)}</div>
                </div>
            </div>
        `;
        container.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
        return;
    }

    // For success status codes, show everything
    // Build headers HTML
    let headersHtml = '';
    response.headers.forEach((value, key) => {
        headersHtml += `<div class="header-row"><span class="header-key">${escapeHtml(key)}:</span> <span class="header-value">${escapeHtml(value)}</span></div>`;
    });

    // Build response HTML
    container.innerHTML = `
        <h3>Response Details</h3>
        <div class="response-header">
            <div class="response-item">
                <label>Method</label>
                <div class="value">${escapeHtml(method)}</div>
            </div>
            <div class="response-item ${statusClass}">
                <label>Status Code</label>
                <div class="value">${response.status} ${escapeHtml(response.statusText)}</div>
            </div>
            <div class="response-item">
                <label>Duration</label>
                <div class="value">${duration} ms</div>
            </div>
            <div class="response-item">
                <label>URL</label>
                <div class="value" style="word-break: break-all;">${escapeHtml(url)}</div>
            </div>
        </div>
        
        <div class="response-section">
            <h4>Response Headers</h4>
            <div class="headers-list">
                ${headersHtml || '<p style="color: var(--text-secondary);">No headers</p>'}
            </div>
        </div>
        
        <div class="response-section">
            <h4>Response Body</h4>
            <div class="response-body">
                <pre>${escapeHtml(body)}</pre>
            </div>
        </div>
    `;

    container.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
}

// Error display
function displayError(method, url, error, duration) {
    const container = document.getElementById('response-container');
    if (!container) return;

    container.classList.add('show');
    container.innerHTML = `
        <h3>Request Error</h3>
        <div class="response-header">
            <div class="response-item">
                <label>Method</label>
                <div class="value">${escapeHtml(method)}</div>
            </div>
            <div class="response-item status-error">
                <label>Error</label>
                <div class="value">Request Failed</div>
            </div>
            <div class="response-item">
                <label>Duration</label>
                <div class="value">${duration} ms</div>
            </div>
            <div class="response-item">
                <label>URL</label>
                <div class="value" style="word-break: break-all;">${escapeHtml(url)}</div>
            </div>
        </div>
        <div class="response-section">
            <h4>Error Message</h4>
            <div class="response-body">
                <pre>${escapeHtml(error.message || error.toString())}</pre>
            </div>
        </div>
    `;

    container.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
}

// Escape HTML to prevent XSS
function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

// Make HTTP request
async function makeRequest(method, url, options = {}) {
    const startTime = performance.now();

    try {
        const fetchOptions = {
            method: method,
            headers: options.headers || {},
        };

        if (options.body) {
            fetchOptions.body = options.body;
        }

        const response = await fetch(url, fetchOptions);
        const endTime = performance.now();
        const duration = Math.round(endTime - startTime);

        // Get response body
        let body = '';
        const contentType = response.headers.get('content-type') || '';
        
        if (contentType.includes('application/json')) {
            try {
                const json = await response.json();
                body = JSON.stringify(json, null, 2);
            } catch (e) {
                body = await response.text();
            }
        } else {
            body = await response.text();
        }

        displayResponse(method, url, response, body, duration);
        return { success: true, response, body };

    } catch (error) {
        const endTime = performance.now();
        const duration = Math.round(endTime - startTime);
        displayError(method, url, error, duration);
        return { success: false, error };
    }
}

// GET Request Handler
function handleGetRequest(event) {
    event.preventDefault();
    const form = event.target;
    const path = form.path.value;
    const baseUrl = form.baseUrl ? form.baseUrl.value : 'http://localhost:8080';
    const url = baseUrl + path;

    makeRequest('GET', url);
}

// POST Request Handler (Text)
function handlePostRequest(event) {
    event.preventDefault();
    const form = event.target;
    const path = form.path.value;
    const baseUrl = form.baseUrl ? form.baseUrl.value : 'http://localhost:8080';
    const textContent = form.textContent ? form.textContent.value : '';
    const url = baseUrl + path;

    makeRequest('POST', url, {
        headers: {
            'Content-Type': 'text/plain'
        },
        body: textContent
    });
}

// DELETE Request Handler
function handleDeleteRequest(event) {
    event.preventDefault();
    const form = event.target;
    const path = form.path.value;
    const baseUrl = form.baseUrl ? form.baseUrl.value : 'http://localhost:8080';
    const url = baseUrl + path;

    if (!confirm(`Are you sure you want to DELETE: ${url}?`)) {
        return;
    }

    makeRequest('DELETE', url);
}

// Test Redirect
function testRedirect(type) {
    const baseUrl = document.getElementById('redirect-base-url')?.value || 'http://localhost:8080';
    let path = '';

    switch(type) {
        case '301':
            path = '/redirect-301';
            break;
        case '302':
            path = '/redirect-302';
            break;
        case '307':
            path = '/redirect-307';
            break;
        case '308':
            path = '/redirect-308';
            break;
        default:
            path = '/redirect';
    }

    const url = baseUrl + path;
    makeRequest('GET', url);
}

// Test Error Page
function testErrorPage(code) {
    const baseUrl = document.getElementById('error-base-url')?.value || 'http://localhost:8080';
    const path = `/errors/${code}.html`;
    const url = baseUrl + path;

    makeRequest('GET', url);
}

// Trigger custom error code
function triggerError(event) {
    event.preventDefault();
    const form = event.target;
    const code = form.errorCode.value;
    const baseUrl = form.baseUrl ? form.baseUrl.value : 'http://localhost:8080';
    const path = `/trigger-error/${code}`;
    const url = baseUrl + path;

    makeRequest('GET', url);
}

// Initialize page
document.addEventListener('DOMContentLoaded', function() {
    // Highlight active nav
    const currentPage = window.location.pathname.split('/').pop() || 'index.html';
    const navLinks = document.querySelectorAll('.nav-btn');
    
    navLinks.forEach(link => {
        const linkPage = link.getAttribute('href').split('/').pop();
        if (linkPage === currentPage) {
            link.classList.add('active');
        }
    });

    // Add form event listeners if they exist
    const getForm = document.getElementById('get-form');
    if (getForm) {
        getForm.addEventListener('submit', handleGetRequest);
    }

    const postTextForm = document.getElementById('post-text-form');
    if (postTextForm) {
        postTextForm.addEventListener('submit', handlePostRequest);
    }


    const deleteForm = document.getElementById('delete-form');
    if (deleteForm) {
        deleteForm.addEventListener('submit', handleDeleteRequest);
    }

    const errorTriggerForm = document.getElementById('error-trigger-form');
    if (errorTriggerForm) {
        errorTriggerForm.addEventListener('submit', triggerError);
    }
});
