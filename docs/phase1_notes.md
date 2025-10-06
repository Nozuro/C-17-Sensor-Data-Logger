| C++ Feature                   | FreeRTOS Equivalent     | Purpose              |
| ----------------------------- | ----------------------- | -------------------- |
| `std::thread`                 | `xTaskCreate`           | Concurrent execution |
| `std::mutex`                  | `xSemaphoreCreateMutex` | Mutual exclusion     |
| `std::condition_variable`     | Queue/Semaphore         | Task signaling       |
| `std::this_thread::sleep_for` | `vTaskDelay`            | Timing control       |
