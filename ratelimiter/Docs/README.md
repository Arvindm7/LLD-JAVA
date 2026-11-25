# Rate Limiter → Distributed Rate Limiter

Interview format: **20 min coding + 15 min system design**. The classic follow-up chain is:
*"Code a rate limiter" → "Now make it work across many servers."*
This folder prepares you for **both halves of that one question**.

## Study order (≈ 3–4 hours total)

| # | File | What you get | Time |
|---|------|--------------|------|
| 1 | [01_concepts.md](01_concepts.md) | What/why, the 4 algorithms, trade-offs | 30 min |
| 2 | [02_coding_round_walkthrough.md](02_coding_round_walkthrough.md) | Line-by-line explanation of the Java code + what to say aloud | 60 min |
| 3 | `code/*.java` | Runnable implementations + tests | 45 min (type them yourself!) |
| 4 | [03_distributed_design.md](03_distributed_design.md) | Redis, Lua, sharding, failure modes, multi-region | 45 min |
| 5 | [04_how_to_communicate.md](04_how_to_communicate.md) | Minute-by-minute scripts for both rounds | 30 min |
| 6 | [05_qa_cheatsheet.md](05_qa_cheatsheet.md) | Follow-up questions with model answers + 1-page revision | 30 min |

## Run the code

```powershell
cd "d:\DSA and prep for placement\LLD\rate limiter\code"
javac *.java
java RateLimiterDemo
```

## The 30-second mental model (memorize)

> A rate limiter answers **one question**: *"Should I allow this request from this key right now?"*
> - **Key** = who we limit (user, API key, IP, tenant).
> - **Algorithm** = how we count (token bucket, sliding window …).
> - **State** = per-key counters, stored in memory (single node) or Redis (distributed).
> - **Decision** = allow / reject (HTTP 429 + `Retry-After`).
>
> Single-node: `ConcurrentHashMap<key, state>` + lock per key.
> Distributed: move state to Redis, make check-and-update **atomic** (Lua script).

## context (use it in the interview!)

runs multi-tenant cloud products (Jira, Confluence, Bitbucket). Real concerns:
- **Per-tenant (per-site) fairness** – one noisy customer must not hurt others.
- **Different limits per plan/endpoint** – e.g. REST API vs. webhooks.
- **Cost-based limiting** – an expensive search "costs" more tokens than a GET.
- Mentioning *noisy neighbour*, *tenant isolation*, *429 + Retry-After* shows product awareness.
