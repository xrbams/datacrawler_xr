**DataCrawler_XR**
==================

**DataCrawler_XR** is a high-performance, concurrent web crawler in C++ designed for efficient web scraping and data extraction.

**Key Features**
----------------

-   **Multi-threaded Crawling**: Concurrent fetching of pages using C++ threads.

-   **Rate Limiting**: Implemented using token bucket to throttle requests.

-   **HTML Parsing**: Uses **Gumbo** to extract data from raw HTML.

-   **Robots.txt Handling**: Automatically checks for crawl restrictions.

-   **Customizable Parsing**: Extendable to add custom scraping logic.

-   **Distributed Crawling (Optional)**: Scalable using message queues (e.g., RabbitMQ).

**Technologies Used**
---------------------

-   **C++17**: For performance-critical components.

-   **libcurl**: Non-blocking HTTP requests.

-   **Gumbo**: HTML5 parser for extracting content.

-   **Threading**: Native C++ threads for concurrency.

-   **CMake**: Build system.

**Setup**
---------

### Prerequisites

-   C++17+ compiler (e.g., GCC, Clang)

-   **CMake** (3.10+)

-   **libcurl**

-   **Gumbo**

### Steps

1.  **Clone the Repository**

    bash

    CopyEdit

    `git clone https://github.com/xrbams/datacrawler_xr.git
    cd datacrawler_xr`

2.  **Install Dependencies**

    bash

    CopyEdit

    `sudo apt-get install libcurl4-openssl-dev libgumbo-dev`

3.  **Build the Project**

    bash

    CopyEdit

    `mkdir build
    cd build
    cmake ..
    make`

4.  **Run the Crawler**

    bash

    CopyEdit

    `./datacrawler_xr`

**Core Components**
-------------------

-   **Crawler**: Manages thread pool and URL queue. Controls rate limits and depth.

-   **Downloader**: Uses **libcurl** for fetching pages asynchronously.

-   **Parser**: Uses **Gumbo** to parse HTML and extract links or data.

-   **Robots.txt Parser**: Checks if URLs are allowed to be crawled.

**Customization**
-----------------

-   Modify the `Parser` module for custom data extraction.

-   Set up distributed crawling by configuring message queues like RabbitMQ.