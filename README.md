# 🧠 Multithreaded Task Scheduler

A **lightweight multithreaded task scheduling system** built in **C++ and Python**, designed to simulate **OS-level process scheduling** (like Round-Robin and Priority Scheduling).
This system enables users to **submit, execute, monitor, and cancel** background jobs in real-time via a **RESTful API**, while the C++ scheduler handles concurrent execution through efficient **thread pooling and synchronization**.

---

## 🚀 Features

- 🧵 **True Concurrency** using C++17 `std::thread` and condition variables for task execution.
- ⚙️ **Priority-based scheduling** and extendable Round-Robin simulation.
- 🔄 **Real-time job monitoring** and control through a Flask REST API.
- 💾 **Persistent storage** of job metadata, outputs, and errors using SQLite.
- 🧱 **Modular architecture** — backend (Python) and scheduler (C++) communicate via a shared database.
- 🔐 **Safe concurrency management** using mutexes and thread-safe queues.

---

## 🧩 Architecture Overview

```
┌────────────────────────────┐
│        REST API (Flask)    │
│  • /submit (POST)          │
│  • /status/<id> (GET)      │
│  • /cancel/<id> (DELETE)   │
│  • /tasks (GET)   │
└──────────────┬─────────────┘
               │ HTTP/JSON
               ▼
┌────────────────────────────┐
│     Task Scheduler (C++)   │
│  • Thread pool (workers)   │
│  • Scheduling queue        │
│  • Mutex & condition var   │
└──────────────┬─────────────┘
               │
               ▼
┌────────────────────────────┐
│         SQLite DB          │
│  • Task metadata           │
│  • Status logs             │
│  • Output/error tracking   │
└────────────────────────────┘
```

---

## ⚙️ Technologies Used

| Technology | Purpose | Reason for Use |
|-------------|----------|----------------|
| **C++17** | Core scheduler engine | Provides low-level thread management, high performance, and fine-grained concurrency control using `std::thread`, `mutex`, and `condition_variable`. |
| **Python (Flask)** | REST API backend | Simple, lightweight, and quick to set up for exposing job control endpoints. |
| **SQLite** | Persistent storage | Fast, file-based database ideal for lightweight, concurrent systems. |
| **CMake** | C++ build automation | Portable build system for managing multi-file C++ projects. |
| **ThreadPool pattern** | Worker management | Efficiently handles multiple concurrent jobs with controlled thread utilization. |
| **Mutex & Condition Variables** | Synchronization primitives | Ensure thread-safe operations without data races or deadlocks. |

---

## 🧮 Concurrency Design

- The **scheduler** maintains a **thread pool** that continuously polls the database for new pending tasks.
- Each worker thread picks up available jobs and executes them asynchronously.
- **Mutex locks** and **condition variables** are used to ensure safe access to the shared task queue.
- **Priority-based scheduling** determines which tasks get executed first, and the design can be extended to support **Round-Robin** time-slicing.
- Multiple jobs run **simultaneously** across threads — providing true concurrency and parallelism across CPU cores.

---

## 🧰 Project Setup

### 🧾 1️⃣ Clone the Repository
```bash
git clone https://github.com/JayJoshi4520/multithreaded-task-scheduler.git
cd Multithreaded-Task-Scheduler
```

---

### 🧱 2️⃣ Set Up the Backend (Flask API)

Navigate to the backend directory:
```bash
cd backend
```

Create and activate a virtual environment:
```bash
python3 -m venv venv
source venv/bin/activate   # On macOS/Linux
venv\Scripts\activate      # On Windows
```

Install dependencies:
```bash
pip install -r requirements.txt
```

---

### 💾 3️⃣ Create the Data Directory
From the root of the project:
```bash
mkdir -p data
```

---

### ⚙️ 4️⃣ Build the Scheduler (C++)
Go to the scheduler directory:
```bash
cd scheduler
mkdir build && cd build
cmake ..
make -j4
```

> ## Note: Scheduler build is already been complete, but for to build after your own further changes please use above instruction to do so.

---

### 🚀 5️⃣ Start the Scheduler
Run the scheduler with the absolute path to your database:
```bash
./scheduler /Users/<your-username>/Desktop/Multithreaded-Task-Scheduler/data/tasks.db 4
```

(Here, `4` = number of worker threads)

---

### 🌐 6️⃣ Start the Backend
Open another terminal, activate your virtual environment, and run:
```bash
cd backend
python app.py
```

Flask will start at:
```
http://127.0.0.1:5000
```

---

## 🧪 Testing the System

You can use **Postman**, **cURL**, or any API testing tool.

### ➕ Submit a new job
```bash
curl -X POST http://127.0.0.1:5000/submit \
     -H "Content-Type: application/json" \
     -d '{"command":"echo Hello World && sleep 2 && echo Done", "priority":5}'
```

### 📋 View all tasks
```bash
curl http://127.0.0.1:5000/tasks | jq .
```

### 🔍 Check a specific task status
```bash
curl http://127.0.0.1:5000/status/1 | jq .
```

### ❌ Cancel a task
```bash
curl -X DELETE http://127.0.0.1:5000/cancel/2
```

---

## 🧠 Example Output

```json
{
  "id": 1,
  "command": "echo Hello World && sleep 2 && echo Done",
  "priority": 5,
  "status": "completed",
  "output": "Hello World\nDone\n"
}
```

---

## 📦 Project Structure

```
multithreaded_task_scheduler/
├── backend/
│   ├── app.py
│   ├── database.py
│   └── requirements.txt
├── scheduler/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── DB.hpp
│   ├── Scheduler.hpp
│   ├── ThreadPool.hpp
│   ├── Utils.cpp
│   └── Utils.hpp
├── data/
│   └── tasks.db
└── README.md
```

---

## 🧩 Future Improvements
- ✅ Support for **true Round-Robin scheduling** with time-slicing.
- ✅ Add **live dashboard** using React or Streamlit for real-time monitoring.
- ✅ Add **Docker support** for containerized deployment.
- ✅ Implement **mid-execution cancellation** via process supervision.

---

## 🧑‍💻 Author
**Jay Joshi**
Software Developer | Systems & Backend Engineering Enthusiast
📧 *jayjoshius810199@gmail.com* | 💼 [LinkedIn](https://www.linkedin.com/in/jay-joshi-b08232200/) | 🌐 [Website](https://jayjoshi.site)

---

## 🪪 License
This project is licensed under the **MIT License** — free for modification and distribution with attribution.
