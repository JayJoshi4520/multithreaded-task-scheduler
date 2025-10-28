from flask import Flask, request, jsonify
from pydantic import BaseModel, ValidationError, Field
from typing import Optional
from flask import Flask, request, jsonify
from pydantic import BaseModel, ValidationError, Field
from typing import Optional
from database import init_db, insert_task, get_task, request_cancel, list_tasks


app = Flask(__name__)
init_db()


class SubmitBody(BaseModel):
    command: str = Field(min_length=1)
    priority: int = Field(default=0, ge=-10, le=10)


@app.route("/submit", methods=["POST"])
def submit():
    try:
        payload = SubmitBody.model_validate(request.get_json(force=True))
    except ValidationError as e:
        return jsonify({"error": e.errors()}), 400

    task_id = insert_task(payload.command, payload.priority)
    return jsonify({"task_id": task_id, "status": "pending"}), 201


@app.route("/status/<int:task_id>", methods=["GET"])
def status(task_id: int):
    row = get_task(task_id)
    if not row:
        return jsonify({"error": "not found"}), 404
    return jsonify(row)


@app.route("/cancel/<int:task_id>", methods=["DELETE"])
def cancel(task_id: int):
    ok = request_cancel(task_id)
    if ok:
        return jsonify({"task_id": task_id, "cancel_requested": True})
    # Either not found or already running/completed
    row = get_task(task_id)
    if not row:
        return jsonify({"error": "not found"}), 404
    return jsonify(
        {"task_id": task_id, "cancel_requested": False, "status": row["status"]}
    )


@app.route("/tasks", methods=["GET"])
def tasks():
    limit = int(request.args.get("limit", 50))
    return jsonify(list_tasks(limit))


if __name__ == "__main__":
    app.run(debug=True)
