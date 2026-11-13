# HTTP routes and HTMX behavior

The application returns **HTML documents or fragments**. These controller routes should not be presented as a stable JSON REST API. Many are intended for HTMX in the browser; names below reflect the `METHOD_ADD` declarations in the C++ controller classes.

## User routes (`Users` controller)

| Method | Route | Parameters | Behavior |
| --- | --- | --- | --- |
| `GET` | `/users/auth/{page}` | Path `page`, generally `signin` or `signup` | Renders sign-in/sign-up template, or logout control when signed in |
| `POST` | `/users/signup` | Form `user`, `passwd` | Hashes password with bcrypt, creates user; sends `201` and `HX-Location: /` on success |
| `POST` | `/users/signin` | Form `user`, `passwd` | Verifies credentials and writes session; successful login returns `HX-Location` targeting `/journals` |
| `POST` | `/users/signout` | Session cookie | Erases account session keys; returns `HX-Location: /` |

The sign-up handler returns `409` for a duplicate username (SQLite extended code `2067`). Sign-in returns `401` when no matching user exists, but **wrong passwords for an existing username currently have no explicit error branch** and may receive an empty/default-success response. This behavior should be corrected, not treated as an authentication guarantee.

## Food catalog (`Foods` controller)

| Method | Route | Parameters | Behavior |
| --- | --- | --- | --- |
| `GET` | `/foods` or `/foods/` | None | Renders searchable catalog page |
| `GET` | `/foods/search?food_name={name}` | Query `food_name` | Performs a case-insensitive-by-default SQLite `LIKE` search (depending on collation) and returns a food-list fragment |
| `GET` | `/foods/name/{name}` | Path food name/filter | Returns matching food-list fragment |
| `GET` | `/foods/id/{food_id}` | Path food ID | Returns a selected item and amount form |

Food catalog routes have no `AuthFilter` in the current controller. There is no endpoint for creating, editing, or deleting records in the shared catalog.

## Journals (`Journals` controller)

All routes below have `AuthFilter` applied, which requires a signed-in session. **Ownership of individual records is not consistently checked.**

| Method | Route | Parameters | Behavior |
| --- | --- | --- | --- |
| `GET` | `/journals` or `/journals/` | Session user ID | Lists the signed-in user's active journals |
| `GET` | `/journals/{journal_id}` | Journal ID | Renders that journal's dated food entries |
| `GET` | `/journals/{journal_id}/{entry_id}` | Journal and entry IDs | Renders logged items and displayed totals for the entry |
| `POST` | `/journals/{journal_id}/new` | Form `entry-date` | Inserts a dated entry with zero totals; returns `201` and `HX-Trigger` on success |
| `POST` | `/journals/food-item/calc/{food_id}` | Form `amount` | Calculates scaled nutritional values and returns an HTML fragment |
| `POST` | `/journals/food-item/add/{food_id}` | Forms `amount`, `entry-id` | Copies selected catalog food as a scaled snapshot into a daily entry; returns `201` and `HX-Trigger` on success |
| `DELETE` | `/journals/food-item/delete/{item_id}` | Food-entry item ID | Deletes logged item; returns `204` and `HX-Trigger` |

There is no active route for updating an entry/item or deleting an entire daily entry. A `delete_entry` declaration is commented out.

## Example request shapes

These illustrate form parameters, not a supported public client SDK:

```http
POST /users/signin HTTP/1.1
Content-Type: application/x-www-form-urlencoded

user=alice&passwd=example-password
```

```http
POST /journals/1/new HTTP/1.1
Content-Type: application/x-www-form-urlencoded

entry-date=2026-11-13
```

```http
POST /journals/food-item/add/5 HTTP/1.1
Content-Type: application/x-www-form-urlencoded

entry-id=12&amount=1.5
```

The `amount` is a **multiplier of the selected catalog quantity**, not necessarily grams. For instance, when a catalog row describes 100 g, an amount of `1.5` records 150 g and 1.5 times the row's protein/calorie values. No robust server-side validation of the range or sign of `amount` currently exists.

## HTMX conventions

- The initial `static/index.html` page loads authentication content into `<main>` using `hx-get`.
- Journal and food navigation replace the main page area or a targeted fragment.
- `HX-Location` is used after signing in/out and signing up to navigate without manually replacing the complete page.
- `HX-Trigger` sends an `itemAdded` event after adding/deleting a journal food or creating a dated entry. Views listen for that event and request refreshed HTML.
- Food search uses debounced HTMX requests (`keyup changed delay:500ms`).

Because most handlers return HTML and depend on browser/session interactions, end-to-end tests should cover rendered fragments and response headers, not merely SQL state.

## Error handling caveats

The project contains TODOs for improving error responses and input handling. Some handlers catch `std::invalid_argument` but do not catch `std::out_of_range` from numeric conversion; other database errors can propagate. For production use, define consistent status codes and user-visible errors, avoid `302` responses containing HTML alert scripts, and validate all untrusted IDs and form values before making database changes.