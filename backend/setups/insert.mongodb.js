// MongoDB Playground
// Use Ctrl+Space inside a snippet or a string literal to trigger completions.

// The current database to use.
use('datacrawler_xr');

// Create a new document in the collection.
db.getCollection('metadata').insertOne({
    url: "https://example.com",
    status: 200,
    timestamp: new Date(),
    headers: { "Content-Type": "text/html" },
    content: {
      title: "Example Page",
      meta_description: "This is a test page",
      text: "Sample extracted content...",
      links: ["https://example.com/about"]
    },
    crawl_metadata: {
      depth: 1,
      parent_url: "https://example.com/start",
      crawled_at: new Date()
    }
});
