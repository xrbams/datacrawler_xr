/* global use, db */
// MongoDB Playground
// Use Ctrl+Space inside a snippet or a string literal to trigger completions.
const database = 'datacrawler_xr';
const metadataCollection = 'crawled_data';
const linksCollection = 'crawled_links';

// Select database
use(database);

// Drop collections if they already exist (for testing purposes)
// Uncomment these if you want a fresh setup each time
// db[metadataCollection].drop();
// db[linksCollection].drop();

// Create 'crawled_data' collection for storing crawled page data
db.createCollection(metadataCollection, {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["url", "status", "timestamp", "headers", "content", "crawl_metadata"],
      properties: {
        url: { bsonType: "string", description: "Webpage URL" },
        status: { bsonType: "int", description: "HTTP status code (200, 404, etc.)" },
        timestamp: { bsonType: "string", description: "ISO 8601 timestamp when the page was crawled" },
        headers: {
          bsonType: "object",
          description: "HTTP response headers",
          properties: {
            "Content-Type": { bsonType: "string", description: "MIME type of content" },
            "Content-Length": { bsonType: "int", description: "Size of response body in bytes" }
          },
          additionalProperties: true // Allow other headers dynamically
        },
        content: {
          bsonType: "object",
          required: ["title", "text", "links"],
          properties: {
            title: { bsonType: "string", description: "Title of the webpage" },
            meta_description: { bsonType: "string", description: "Meta description of the page" },
            text: { bsonType: "string", description: "Extracted text content" },
            links: {
              bsonType: "array",
              description: "List of discovered links",
              items: { bsonType: "string" }
            }
          }
        },
        crawl_metadata: {
          bsonType: "object",
          required: ["depth", "parent_url", "crawled_at"],
          properties: {
            depth: { bsonType: "int", description: "Depth in crawl hierarchy (0 = root)" },
            parent_url: { bsonType: "string", description: "Referrer URL where this page was found" },
            crawled_at: { bsonType: "string", description: "ISO 8601 timestamp of crawl" }
          }
        }
      }
    }
  }
});

// Create 'crawled_links' collection for storing discovered links separately
db.createCollection(linksCollection, {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["url", "parent_url", "discovered_at"],
      properties: {
        url: { bsonType: "string", description: "Discovered link URL" },
        parent_url: { bsonType: "string", description: "Page where this link was found" },
        discovered_at: { bsonType: "string", description: "ISO 8601 timestamp when the link was found" }
      }
    }
  }
});

// 🔹 Ensure indexes for fast lookups
db[metadataCollection].createIndex({ url: 1 }, { unique: true }); // Prevent duplicate pages
db[metadataCollection].createIndex({ status: 1 }); // Query by HTTP status
db[metadataCollection].createIndex({ "crawl_metadata.depth": 1 }); // Query by depth

db[linksCollection].createIndex({ url: 1 }); // Fast lookups for visited links
db[linksCollection].createIndex({ parent_url: 1 }); // Query links by parent page

// Confirmation message
print(`✅ Database "${database}" has been set up with collections "${metadataCollection}" and "${linksCollection}".`);
