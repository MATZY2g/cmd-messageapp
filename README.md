# termmsg — Anonymous Terminal Messenger (C, No External Libs)

## ⚠️ Disclaimer  
This project is an **educational prototype**.  
The current implementation uses a **toy XOR cipher**, which is **not cryptographically secure**.  
Do **not** use this app for real secrets.  
For production-ready use, replace it with a proven cipher (e.g., **ChaCha20-Poly1305** or **AES-GCM**).  

---

## ✨ Features
- **One user per PC**
  - On first run, the app prompts you to generate a personal key.
  - Key is stored locally at `~/.termmsg/key.hex`.
  - User identity is automatically built as **`<PC_NAME>@<LOCATION>`**.

- **Anonymous Chat**
  - Users share their **public hex key** to connect.
  - No central accounts, no emails, no usernames — just keys.

- **End-to-End Encryption**
  - Messages are encrypted with the recipient’s key before leaving your PC.
  - The relay server **cannot decrypt** messages — it only stores ciphertext.
  - Messages are **one-time**: they disappear once delivered.

- **Database Support**
  - Messages are stored in an **SQL database** on the server.
  - Easier to query, manage, and clean up.

- **Command-Line Interface**
  - Entirely terminal-based.
  - Simple commands to send, receive, and save conversations.

---

## 🛠️ Build (POSIX)

```bash
cc -O2 -Wall -Wextra -o termmsg termmsg.c
```

---

## 🚀 Usage

### First Run
```bash
./termmsg
```
- If no key exists → prompts to generate a new key.  
- If key exists → greets the user (`Hi, <PC_NAME>@<LOCATION>!`) and asks:  
  ```
  Enter recipient key (hex) to start chatting:
  ```

### Start the Server
```bash
./termmsg server <host> <port>
```
Starts the relay server, which stores encrypted messages in the SQL database.

### Send a Message
```bash
./termmsg send <host> <port> <recipient_key_hex> "hello from termmsg"
```

### Receive Messages
```bash
./termmsg recv <host> <port> [--save path]
```
- Fetches and decrypts all pending messages.
- Messages are deleted from the server after delivery.
- Use `--save <file>` to store conversation locally.

---

## 🔗 Protocol (line-based over TCP)

**Send Message**
```
SEND <recipient_key_hex> <hex_ciph_len>\n
<hex(ciphertext)>\n
```
Response:
```
OK\n
ERR\n
```

**Receive Messages**
```
RECV <your_key_hex>\n
```
Response:
```
COUNT <n>\n
MSG <hex_len>\n
<hex(ciphertext)>\n
...
```

---

## 📦 Storage
- Server stores encrypted messages in a **SQL database** table (e.g., `messages`).
- Messages are automatically **deleted after delivery**.

---

## 🧭 Roadmap
- [ ] Replace toy XOR cipher with **ChaCha20** or **AES** (self-contained, no libs).  
- [ ] Auto-detect **PC name + location** for usernames.  
- [ ] Interactive chat loop (`/send`, `/recv`, `/quit`).  
- [ ] Message expiration timers.  
- [ ] Windows support (Winsock + CryptGenRandom).  

---

💡 The goal of `termmsg` is to make a **minimal, anonymous, terminal-only messenger** where:  
- There’s **one user per PC**.  
- Messages are **end-to-end encrypted**.  
- Only the intended recipient can read them.  
