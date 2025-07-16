# Copyright mzoesch. All rights reserved.

import os
import json
import hmac
import hashlib
from fastapi import Header
from fastapi import FastAPI
from fastapi import Request
from fastapi import HTTPException


app = FastAPI()


# Secret transmitted by the GitHub webhook imported from the local environment
secret_x_hub_signature = os.getenv('SECRET_X_HUB_SIGNATURE')
if secret_x_hub_signature is None:
    raise ValueError('Environment variable SECRET_X_HUB_SIGNATURE is not set')
secret_x_hub_signature = secret_x_hub_signature.encode('utf-8')


@app.get('/')
def read_root():
    return {'Hello': 'World'}


@app.post('/projects/s-jafg/post-push')
async def handle_post_req_sjafg_post_push(
    request: Request,
    x_hub_signature_256: str = Header(None)
    ):

    if x_hub_signature_256 is None:
        raise HTTPException(status_code=400, detail='Missing X-Hub-Signature-256 header')

    body = await request.body()
    computed_signature = 'sha256=' + hmac.new(secret_x_hub_signature, body, hashlib.sha256).hexdigest()

    if not hmac.compare_digest(computed_signature, x_hub_signature_256):
        raise HTTPException(status_code=403, detail='Forbidden')

    json_payload = await request.json()

    with open('_xvar.txt', 'w') as f:
        f.write(x_hub_signature_256)

    with open('_var.txt', 'w') as f:
        f.write(json.dumps(json_payload))

    return \
    {
        'status': 'Webhook received and verified'
    }
