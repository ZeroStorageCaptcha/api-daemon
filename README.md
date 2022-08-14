# ZSC API daemon
Zero Storage Captcha selfhosted REST API service.

## Compile

```
sudo apt install git qtdeclarative5-dev
git clone --recursive https://github.com/ZeroStorageCaptcha/api-daemon.git zero-captcha-daemon
cd zero-captcha-daemon
qmake && make
```

## Run

```
-a --address  Address to bind (127.0.0.1 by default)
-p --port     Port to bind (7697 by default)
-t --threads  Working threads (hardware value by default)
```

For Linux systems with systemd check `systemd` folder which contains service file example.

## API

GET request, JSON response

**Each response contains a boolean "status" that indicates the logic success of the operation. If !status, read "message" field.**

1. Generate captcha
   ```
   -> /generate?length=CAPTCHA_TEXT_LENGTH&difficulty=0|1|2
   <- { "token": "CAPTCHA_TOKEN", "png": "BASE64_ENCODED_PICTURE" }
   ```
2. Validate captcha
   ```
   -> /validate?answer=CAPTCHA_ANSWER&token=CAPTCHA_TOKEN
   <- { "valid": true|false }
   ```
3. Settings

    3.1 Tokens case sensitive to captcha answer:
    ```
     -> /settings?case_sensitive=enable|disable
    ```
    3.2 Numbers only mode:
    ```
    -> /settings?number_mode=enable|disable
    ```
## Example

1. Generate
   ```
   Client: /generate?length=4&difficulty=1
   Server: 
   {
     "status": true,
     "png": "iVBORw0KGgoAAAANSU <...> AAAAAElFTkSuQmCC",
     "token":" i2oefBw6mswaORIphgDcY7GwnS_532"
   }
   ```
2. Validate
   ```
   Client: /validate?answer=iuda1&token=i2oefBw6mswaORIphgDcY7GwnS_532
   Server: 
   {
     "status": true,
     "valid": false
   }
   ```
