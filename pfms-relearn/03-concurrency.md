# Week 4: Concurrency: threads, mutexes, condition variables

**Goal for the week:** understand threads well enough that A2's requirement, *"`run()` is non-blocking, platforms execute simultaneously, `status()` reports progress"*, feels natural.

The main source of bugs in A2 is shared data touched by two threads. This week is about making that safe and avoiding busy waits.

---

## Day 22: `std::thread` basics

**Learn**
- `std::thread t(&Class::method, this);`, `join()`, `detach()` (and why you almost never want `detach`).
- A `std::thread` that is still joinable when destroyed calls `std::terminate`.

**Do:** start 3 threads that each print their id 5 times with a 100 ms sleep. Then wrap one in a class that starts a thread in a `start()` method and joins it in the destructor.

**Done when:** the class version exits cleanly with no crash, even if you never call a "stop" function.

---

## Day 23: Data races and `std::mutex`

**Learn**
- What a data race is; why `counter++` from two threads loses updates.
- `std::mutex`, `std::lock_guard`, `std::unique_lock`. Keep critical sections short.

**Do:** two threads each increment a shared counter 1,000,000 times. Run without a mutex (watch it go wrong), then with `lock_guard`, then with `std::atomic<int>`.

**Done when:** you get the wrong answer without protection and exactly 2,000,000 with either fix.

---

## Day 24: `std::condition_variable` (no busy waiting)

**Learn**
- `cv.wait(lock, predicate)`, `notify_one`, `notify_all`, spurious wakeups.
- Why `while(!ready) {}` is marked down in A2 ("avoiding busy waits").

**Do:** a producer thread pushes "odometry" structs into a `std::deque` every 10 ms; a consumer thread waits on a condition variable and processes each one.

**Done when:** CPU usage (check `top`) stays low while the consumer waits, and no data is lost.

---

## Day 25: Non-blocking `run()` + `status()`

This is the core A2 pattern. Build it on your `MockAckerman`.

**Do**
```cpp
class Platform {
public:
  void setGoals(std::vector<Point> goals);  // stores goals (lock!)
  bool run();                               // starts control thread, returns immediately
  PlatformStatus status();                  // IDLE or RUNNING (thread-safe read)
  ~Platform();                              // signal stop + join
private:
  void controlLoop();                       // runs in the thread: goals one by one
  std::thread thread_;
  std::mutex mtx_;
  std::atomic<bool> running_{false};
  std::atomic<bool> stop_{false};
  std::vector<Point> goals_;
};
```

**Done when:** `run()` returns in < 1 ms, `main` polls `status()` every 200 ms and prints progress, and the program exits cleanly when goals are done **and** when you Ctrl+C mid-run.

---

## Day 26: Progress reporting + shared estimates

**Do**
- Compute total expected distance at start (sum of arc lengths through the goals).
- `status()` / progress = distance travelled ÷ total expected.
- As A2 suggests, **re-estimate** remaining distance as you reach each goal so it ends at exactly 100%.

**Done when:** progress rises roughly smoothly and finishes at 100, never above.

---

## Day 27: Two platforms at once

**Do:** run two `MockAckerman` platforms in their own threads, controlled by a `Mission` that owns `std::vector<ControllerInterface*>` and has its own non-blocking `run()` and `status()` (mission progress = combined distance ÷ combined total).

**Done when:** timestamps in your output show both platforms moving at the same time, and mission status reaches 100 when the slower one finishes.

---

## Day 28: Buffer / review

- Self-check:
  1. What happens if a `std::thread` member is destroyed without `join()`?
  2. Why use `atomic<bool>` for a stop flag but a mutex for a `vector<Point>`?
  3. Where is the busy wait in `while(status()==RUNNING){}` and how would you remove it?
- ⭐ Stretch: run your Day 25 code under ThreadSanitizer (`-fsanitize=thread`) and fix anything it reports.
