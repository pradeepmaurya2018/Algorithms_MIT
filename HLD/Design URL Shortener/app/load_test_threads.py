import asyncio, aiohttp, time

URL = "http://localhost:8080/shorten"
payload = {"longUrl": "https://example.com"}

SEM = asyncio.Semaphore(20)  # control concurrency

async def hit(session):
    async with SEM:
        async with session.post(URL, json=payload) as r:
            await r.text()

async def main():
    start = time.time()
    async with aiohttp.ClientSession() as session:
        tasks = [hit(session) for _ in range(2000)]
        await asyncio.gather(*tasks)
    end = time.time()

    print("Total time:", end - start)
    print("Throughput (req/sec):", 2000 / (end - start))

asyncio.run(main())
