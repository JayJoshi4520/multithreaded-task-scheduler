import sqlite3
from pathlib import Path
from typing import Any, Dict

DB_PATH = Path(__file__).resolve().parent.parent / "data" / "tasks.db"
DB_PATH.parent.mkdir(parents=True, exist_ok=True)


def get_conn():
    conn = sqlite3.connect(DB_PATH, check_same_thread=False)
    conn.row_factory = sqlite3.Row
    return conn


def init_db():
    with get_conn() as conn:
        conn.execute("PRAGMA journal_mode=WAL;")
        conn.execute("""
        CREATE TABLE IF NOT EXISTS tasks(
          id INTEGER PRIMARY KEY AUTOINCREMENT,
          command TEXT NOT NULL,
          priority INTEGER NOT NULL DEFAULT 0,
          status TEXT NOT NULL DEFAULT 'pending',
          cancel_requested INTEGER NOT NULL DEFAULT 0,
          created_at TEXT DEFAULT (datetime('now')),
          started_at TEXT,
          finished_at TEXT,
          output TEXT,
          error TEXT
        );
        """)
        conn.commit()


def insert_task(command: str, priority: int) -> int:
    with get_conn() as conn:
        cur = conn.execute(
            "INSERT INTO tasks(command, priority, status) VALUES(?,?, 'pending');",
            (command, priority),
        )
        conn.commit()
        return cur.lastrowid if cur.lastrowid is not None else -1


def get_task(task_id: int) -> Dict[str, Any] | None:
    with get_conn() as conn:
        cur = conn.execute("SELECT * FROM tasks WHERE id=?;", (task_id,))
        row = cur.fetchone()
        return dict(row) if row else None


def request_cancel(task_id: int) -> bool:
    with get_conn() as conn:
        cur = conn.execute(
            "UPDATE tasks SET cancel_requested=1 WHERE id=? AND status='pending';",
            (task_id,),
        )
        conn.commit()
        return cur.rowcount == 1


def list_tasks(limit: int = 50):
    with get_conn() as conn:
        cur = conn.execute(
            "SELECT id, command, priority, status, created_at, started_at, finished_at FROM tasks ORDER BY id DESC LIMIT ?;",
            (limit,),
        )
        return [dict(r) for r in cur.fetchall()]
