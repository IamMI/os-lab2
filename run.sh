#!/bin/bash

echo "Now run scripts.py!"
python scripts.py
echo "Now run draw.py!"
conda init
conda activate MMVP
python draw.py