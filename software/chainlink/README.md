# Chainlink software

## Python Code

Run the demo by installing the requirement dependencies outlined in the [requirements.txt](./requirements.txt).

### Running the demo

The demo will just send multiple different words to the splitflap board. You may start the demo by executing:

```bash
python demo.py
```

You may get an output looking like this:

```bash
Available ports:
[ 0] /dev/ttyACM0 - USB Single Serial
[ 1] /dev/ttyAMA0 - ttyAMA0
```

Select which port the demo script should execute.

Now the demo should send different words to the splitflap board with short breaks in between them.
