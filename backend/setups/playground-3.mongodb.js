/* global use, db */
// MongoDB Playground
// Use Ctrl+Space inside a snippet or a string literal to trigger completions.
/* global use, db */
// MongoDB Playground
// Use Ctrl+Space inside a snippet or a string literal to trigger completions.

const database = 'datacrawler_xr';
const collection = 'metadata';

// Create a new database.
use(database);

// Create a new collection.
db.createCollection(collection, {
    validator: {
      $jsonSchema: {
        bsonType: "object",
        required: ["url", "status", "timestamp", "content", "crawl_metadata"],
        properties: {
          url: {
            bsonType: "string",
            description: "Must be a string and is required"
          },
          status: {
            bsonType: "int",
            description: "HTTP status code (e.g., 200, 404, 500)"
          },
          timestamp: {
            bsonType: "date",
            description: "Time the page was crawled"
          },
          headers: {
            bsonType: "object",
            description: "HTTP response headers",
            additionalProperties: true
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
                items: { bsonType: "string" },
                description: "List of links found on the page"
              }
            }
          },
          crawl_metadata: {
            bsonType: "object",
            required: ["depth", "parent_url", "crawled_at"],
            properties: {
              depth: { bsonType: "int", description: "Depth in crawl hierarchy" },
              parent_url: { bsonType: "string", description: "The URL that led to this page" },
              crawled_at: { bsonType: "date", description: "When the page was crawled" }
            }
          }
        }
      }
    }
  });

// Ensure indexes for fast lookups
db[collection].createIndex({ url: 1 }, { unique: true });
db[collection].createIndex({ status: 1 });
db[collection].createIndex({ "crawl_metadata.depth": 1 });

// Confirmation message
print(`Database "${database}" and collection "${collection}" have been created successfully.`);
