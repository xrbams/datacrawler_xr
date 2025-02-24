// Module D: Scheduler

/*
- Select URLs from Module C (Queue) and assign them to Module A (Downloader).
- Apply rate limiting and site-specific policies (respect robots.txt).
- Implement crawl depth limits (so it doesn’t go infinitely deep).

- Task Scheduling: std::thread or Boost.Asio.
- Rate Limiting: Token bucket algorithm (e.g., leaky bucket logic).
- Concurrency Control: Mutex locks or thread-safe queues (e.g., boost::lockfree::queue).

*/