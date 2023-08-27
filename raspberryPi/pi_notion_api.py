import os

import requests
from datetime import datetime, timezone


NOTION_KEY = os.getenv("NOTION_INTEGRATION_KEY")
NOTION_BODYWEIGHT_DATABASE_ID = os.getenv("NOTION_BODYWEIGHT_DATABASE_ID")
NOTION_SHOPPING_DATABASE_ID = os.getenv("NOTION_SHOPPING_DATABASE_ID")
NOTION_MEMOS_DATABASE_ID = os.getenv("NOTION_MEMOS_DATABASE_ID")

headers = {
    "Authorization": "Bearer " + NOTION_KEY,
    "content-type": "application/json",
    "Notion-Version": "2022-06-28",
}


def get_notion_pages(database_id):
    url = f"https://api.notion.com/v1/databases/{database_id}/query"

    payload = {"page_size": 100}
    response = requests.post(url, json=payload, headers=headers)
    data = response.json()

    import json
    with open('db.json', 'w', encoding='utf8') as f:
        json.dump(data, f, ensure_ascii=False, indent=4)

    results = data["results"]
    return results


def create_notion_page(database_id, data: dict):
    create_url = "https://api.notion.com/v1/pages"

    payload = {"parent": {"database_id": database_id}, "properties": data}

    res = requests.post(create_url, headers=headers, json=payload)
    print(res.status_code)
    return res.status_code


def update_notion_page(page_id: str, data: dict):
    url = f"https://api.notion.com/v1/pages/{page_id}"

    payload = {"properties": data}

    res = requests.patch(url, json=payload, headers=headers)
    print(res.status_code)
    return res


def delete_notion_page(page_id: str):
    url = f"https://api.notion.com/v1/pages/{page_id}"

    payload = {"archived": True}

    res = requests.patch(url, json=payload, headers=headers)
    print(res.status_code)
    return res


# 1. test page reading
# pages = get_notion_pages(NOTION_SHOPPING_DATABASE_ID)
# for page in pages:
#     page_id = page["id"]
#     props = page["properties"]
#     name = props["Name"]["title"][0]["text"]["content"]
#     date = props["Date"]["date"]["start"]
#     print(name, date)


# 2. test page creation
# name = "Banana"
# record_date = datetime.now().strftime("%Y-%m-%d")

# data = {
#     "Name": {"title": [{"text": {"content": name}}]},
#     "Date": {"date": {"start": record_date, "end": None}}
# }
# create_notion_page(NOTION_SHOPPING_DATABASE_ID, data)


# 3. page_id = ""
# delete_notion_page(page_id)