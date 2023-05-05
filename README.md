# ZSC API daemon
Zero Storage Captcha selfhosted REST API service.

## Compile

Qt5 variant, Debian 11

```
sudo apt install git build-essential qtdeclarative5-dev
git clone --recursive https://github.com/ZeroStorageCaptcha/api-daemon.git zero-captcha-daemon
cd zero-captcha-daemon
qmake && make
```

## Run

Argument list

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
   -> /generate
   <- { "token": "CAPTCHA_TOKEN", "png": "BASE64_ENCODED_PICTURE" }
   ```
2. Validate captcha
   ```
   -> /validate?answer=CAPTCHA_ANSWER&token=CAPTCHA_TOKEN
   <- { "valid": true|false }
   ```
3. Settings

    3.1 Tokens case sensitive to captcha answer (`disable` by default):
    ```
     -> /settings?case_sensitive=enable|disable
    ```
    3.2 Numbers only mode (`disable` by default):
    ```
    -> /settings?number_mode=enable|disable
    ```
    3.3 Difficulty (`1` by default):
    ```
    -> /settings?difficulty=0|1|2
    ```
    3.4 Text length (number greater than 0, `5` by default):
    ```
    -> /settings?length=6
    ```
    3.5 Cache capacity (`2048` by default, `0` for disable):
    ```
    -> /settings?cache_capacity=4096
    ```

## Example

1. Generate
   ```
   Client: /generate
   Server: 
   {
     "status": true,
     "png": "iVBORw0KGgoAAAANSU <...> AAAAAElFTkSuQmCC",
     "token":" i2oefBw6mswaORIphgDcY7GwnS_Aq"
   }
   ```
2. Validate
   ```
   Client: /validate?answer=iuda1&token=i2oefBw6mswaORIphgDcY7GwnS_Aq
   Server: 
   {
     "status": true,
     "valid": false
   }
   ```
