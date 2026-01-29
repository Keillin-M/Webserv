## 🗓️ High-Confidence 4-Week Plan

### 🧱 **Week 1 — Foundation (ALL TOGETHER)**

**Goal:** A running server skeleton that accepts clients and responds to a basic GET.

#### Do together (this is crucial):

* Decide **architecture**
* Decide **class responsibilities**
* Decide **poll/select strategy**
* Decide **config data structures**
* Set **coding rules**

#### Deliverables by end of Week 1:

✅ Non-blocking server
✅ `poll()` loop
✅ Accept multiple clients
✅ Minimal HTTP parsing
✅ Hardcoded `GET /` → `200 OK`

> If this works, you are already past the scariest part.

---

### ⚙️ **Week 2 — Features Split (Parallel Work)**

**Goal:** Functional server with config, methods, and error handling.

#### Person 1 — Network & Stability

* Multiple servers / ports
* Client lifecycle
* Partial reads / writes
* Timeouts
* Proper close & cleanup

#### Person 2 — HTTP Core

* Full request parsing
* GET / POST / DELETE
* Status codes
* Error responses
* File serving

#### Person 3 — Config System

* Config grammar
* Location matching
* Allowed methods
* Roots & index
* Error pages

#### End of Week 2:

✅ Config file works
✅ Methods work
✅ Errors behave like nginx

You should be able to:

```bash
curl -X POST localhost:8080/upload
curl -X DELETE localhost:8080/file
```

---

### 🔥 **Week 3 — CGI, Edge Cases & Polish**

**Goal:** Everything required by subject + stability.

#### Main focus:

* CGI execution
* Environment variables
* Upload handling
* Directory listing (autoindex)
* Security checks
* Memory leaks
* Stress testing

⚠️ CGI often eats time — start it early in Week 3.

#### End of Week 3:

✅ All subject requirements done
✅ Stable under load
✅ No crashes
✅ Memory clean (`valgrind`)

---

### 🧪 **Week 4 — Eval Mode (VERY IMPORTANT)**

**Goal:** Be evaluator-proof.

#### What to do:

* Compare behavior with nginx
* Test wrong configs
* Test malformed requests
* Kill CGI mid-execution
* Disconnect clients mid-request
* Read the **evaluation sheet line by line**

#### Practice answers:

Evaluators will ask:

* “Why poll?”
* “How do you avoid blocking?”
* “How does CGI work?”
* “How do you match locations?”
* “What happens if two clients send data slowly?”

Everyone should be able to answer.

---

## 🎯 Key Success Rules

### 🟢 Rule 1 — Minimal first, perfect later

Don’t over-engineer in Week 1.

### 🟢 Rule 2 — Merge daily

Webserv punishes late merges.

### 🟢 Rule 3 — Everyone understands everything

Even if ownership is split.

### 🟢 Rule 4 — Behavior > features

Evaluators care more about **correct HTTP behavior** than fancy extras.

---

