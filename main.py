import requests
import json

URL = "https://leetcode.com/graphql"

HEADERS = {
    "Content-Type": "application/json",
    "Accept": "application/json",
    "Referer": "https://leetcode.com/problemset/all/",
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64)",
}

QUERY = """
query problemsetQuestionListV2(
  $categorySlug: String
  $limit: Int
  $skip: Int
) {
  problemsetQuestionListV2(
    categorySlug: $categorySlug
    limit: $limit
    skip: $skip
  ) {
    questions {
      questionFrontendId
      title
      titleSlug
      difficulty
      paidOnly
      topicTags {
        name
        slug
      }
    }
  }
}
"""

def fetch_all_problems(batch_size=100):
    all_problems = []
    skip = 0

    while True:
        payload = {
            "query": QUERY,
            "variables": {
                "categorySlug": "",
                "limit": batch_size,
                "skip": skip
            }
        }

        response = requests.post(URL, headers=HEADERS, json=payload)

        if response.status_code != 200:
            print("HTTP Error:", response.status_code)
            print(response.text)
            break

        data = response.json()

        if "errors" in data:
            print("GraphQL Error:", data["errors"])
            break

        questions = data["data"]["problemsetQuestionListV2"]["questions"]

        if not questions:
            break

        all_problems.extend(questions)
        skip += batch_size

        print(f"Fetched {len(all_problems)} problems")

    return all_problems


if __name__ == "__main__":
    problems = fetch_all_problems()

    with open("leetcode_problems.json", "w", encoding="utf-8") as f:
        json.dump(problems, f, indent=2)

    print(f"\nSaved {len(problems)} problems to leetcode_problems.json")
