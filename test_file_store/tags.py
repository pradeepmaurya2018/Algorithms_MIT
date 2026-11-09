import json
from collections import defaultdict

# Load the existing title -> topics map
with open("problem_title_to_topics.json", "r", encoding="utf-8") as f:
    title_to_topics = json.load(f)

# print(title_to_topics)
# Build: Topic -> Problem Titles map

# topic_to_titles = defaultdict(list)
# topics_titles = defaultdict(list)

title_to_topics_map=defaultdict(list)


for title, topics in title_to_topics.items():
    temp_topics=[]
    for i in range(len(topics)):
        temp_topics.append(topics[i].lower())

    # for item in ["array", "database", "string", "math", "dynamic programming", "hash table", "greedy", ]:
    #     if item in temp_topics: temp_topics.remove(item)



    title_to_topics_map[tuple(sorted(temp_topics))].append(title)

# print(title_to_topics_map)

topics_titles_frequency=[]

for topics, title in title_to_topics_map.items():
    # print(title, len(topics))
    topics_titles_frequency.append((len(title), topics, title ))
topics_titles_frequency.sort(reverse=True)

for i in range(len(topics_titles_frequency)):
    if topics_titles_frequency[i][0]>=10:
        print(topics_titles_frequency[i])

# # Convert defaultdict to normal dict for JSON
# topic_to_titles = dict(topic_to_titles)

# # Save result
# with open("topic_to_problem_titles.json", "w", encoding="utf-8") as f:
#     json.dump(topic_to_titles, f, indent=2)
#
# print(f"Saved {len(topic_to_titles)} topics")
