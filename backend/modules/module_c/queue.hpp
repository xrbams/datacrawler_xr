// Module C: Queue

/*
- Store pending URLs extracted from Module A.
- Prioritize URLs based on importance & freshness.
- Avoid re-crawling the same URLs too often (use a Bloom filter or hash table).

- Priority Queue: std::priority_queue (can use min-heap for URL prioritization).
- Deduplication: Bloom filters (libbf or boost::dynamic_bitset).
- Message Queue (Optional, for distributed crawling): Apache Kafka or RabbitMQ

*/