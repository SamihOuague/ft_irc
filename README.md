*This project has been created as part of the 42 curriculum by \<souaguen>, \<lsouc\>.*

# ft_irc — Internet Relay Chat Server

> Building the internet, one socket at a time.

## Description

`ft_irc` is a fully functional **IRC server written in C++98**, crafted from scratch as part of the 42 curriculum. No frameworks, no shortcuts — just raw sockets, a single `poll()`, and a healthy respect for [RFC 1459](https://datatracker.ietf.org/doc/html/rfc1459).

Real IRC clients can connect to it, chat, join channels, and get kicked out of them. Everything you'd expect from a proper IRC server.

### What it can do

- Handle multiple clients simultaneously — no forking, no blocking, no drama
- Password-protect the server because not everyone deserves an invite
- Full authentication flow: `PASS`, `NICK`, `USER`
- Channel messaging: `JOIN`, `PART`, `PRIVMSG`
- Operator commands for the power-hungry: `KICK`, `INVITE`, `TOPIC`, `MODE`
- Channel modes: `i` (invite-only), `t` (topic lock), `k` (password), `o` (give/take operator), `l` (user limit)
- TCP/IP over IPv4 and IPv6
- Proper packet reassembly — because TCP is a stream, not a mailbox

---

## Instructions

### Requirements

- A computer
- A C++98-compatible compiler (`c++`)
- A Unix-based OS (Linux or macOS)
- [Irssi](https://irssi.org/) as the reference IRC client (or any other IRC client really, we won't judge)

### Compilation

```bash
make
```

Produces the `ircserv` binary. The usual Makefile suspects are all here: `all`, `clean`, `fclean`, `re`.

### Running the server

```bash
./ircserv <port> <password>
```

- `<port>`: The port to listen on.
- `<password>`: Clients must provide this to connect. Choose wisely.

**Example:**
```bash
./ircserv 6667 i_am_dumb
```

### Connecting with Irssi

```bash
irssi
/connect -password i_am_dumb 127.0.0.1 6667
```

From there you can find a cool name, join a channel, and start chatting like it's 1993.

### Quick sanity test with netcat

Want to make sure partial data is handled correctly? Try this:

```bash
nc -C 127.0.0.1 6667
```

Type a command in chunks, flushing each piece with `Ctrl+D`. The server should patiently collect every fragment before processing the full command.

---

## Commands

### 🔐 Connection & Authentication

| Command | Usage | Description |
|--------|-------|-------------|
| `PASS` | `PASS <password>` | Set your password, be creative. |
| `NICK` | `NICK <nickname>` | Claim your new identity. |
| `USER` | `USER <username>` | Tell the server who you actually are. |
| `QUIT` | `QUIT [:<reason>]` | Gracefully disconnect. Leave a dramatic exit message or don't — your call. |

### 💬 Messaging

| Command | Usage | Description |
|--------|-------|-------------|
| `PRIVMSG` | `PRIVMSG <target> :<message>` | Send a message to a user or channel. The backbone of all IRC communication. `<target>` can be a nickname or a `#channel`. |

### 📢 Channels

| Command | Usage | Description |
|--------|-------|-------------|
| `JOIN` | `JOIN <#channel> [key]` | Walk into a channel. If it doesn't exist yet, you just created it — congratulations, you're the operator now. |
| `PART` | `PART <#channel> [:<reason>]` | Leave a channel. Unlike `QUIT`, you can provide a passive-aggressive reason specific to that channel. |
| `TOPIC` | `TOPIC <#channel> [:<topic>]` | View or change the channel topic. Without an argument, just shows the current one. With an argument, you're rewriting history — if you have the right to do so. |
| `INVITE` | `INVITE <nickname> <#channel>` | Personally escort someone into an invite-only channel. Use this power responsibly. |
| `KICK` | `KICK <#channel> <nickname> [:<reason>]` | Forcibly remove someone from a channel. The nuclear option for channel moderation. Always add a reason or don't if it's out of spite. |

### ⚙️ MODE — Channel Settings

`MODE <#channel> <+/-><mode> [parameter]`

The Swiss army knife of channel control. Operators can stack these up or tear them down:

| Mode | Example | Description |
|------|---------|-------------|
| `+i` / `-i` | `MODE #chan +i` | **Invite-only.** The channel goes private — only people you `INVITE` can join. Great for keeping out the opps. |
| `+t` / `-t` | `MODE #chan +t` | **Topic lock.** Only operators can change the topic. Prevent users from setting it to something embarrassing. |
| `+k` / `-k` | `MODE #chan +k secret` | **Channel key (password).** Users must provide the key to join: `JOIN #chan secret`. Remove it with `-k`. |
| `+o` / `-o` | `MODE #chan +o alice` | **Operator privilege.** Crown someone as a channel operator — or take the crown away. Choose your lieutenants wisely. |
| `+l` / `-l` | `MODE #chan +l 42` | **User limit.** Cap the number of users allowed in the channel. Once it's full, newcomers get turned away at the door. |

---

## Resources

### IRC Protocol & Networking

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459) — The OG spec. Dense but essential.
- [RFC 2812 — IRC: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812) — The cleaner follow-up.
- [Modern IRC Client Protocol](https://modern.ircdocs.horse/) — RFC 1459 but written by someone who wanted you to actually understand it.
- [Irssi documentation](https://irssi.org/documentation/) — Reference for the client used during development and testing.

### AI Usage

AI was used to write this README. That's it. The actual code, architecture, and 4am command debuging and cursing were entirely human-powered.

---

*"The key to IRC is that it's fundamentally broken in several ways, and we all agreed to pretend otherwise." — probably someone on IRC, circa 1994*
