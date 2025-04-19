# Amiga Examples

Some small examples about programming the amiga

## Prerequisits

- SAS/C: All examples were compiled with SAS/C
- VS Code (optional)
- WinUAE (optional)

## Configure (Optional)

I use WinUAE to compile and execute the examples. I added some configuration to make the workflow more convinient.

- Programming is done in vscode
- From VS Code you start a task, which is launching a configured WinUAE instance booting up the amiga
- In the booted amiga compile the code and launch the program

### Setup WinUAE

Nowadays it's way more easy to test/program your code with the help of an emulator.
I used WinUAE, but thwere are many other options.

#### Install WinUAE

Download WinUAE

#### Create a Configuration

There are several instructions on the internet how to configure WinUAE. Explaining it, would take too long.

**The important thing is**

Add a harddrive pointing to a folder on your host system.
e.g.

dh2 -> c:\development\workspace\Amiga-Examples

It's important that this mountpath is a parent folder of your workspace where your source code exists.
You will acces those files from the amiga side on this harddrive mount.

Just make sure that you create a configuration, save it and remember the name.
For this explanation we choose: config-micro.uae (a very minimalistic configuration to speed up the boot time)

Start WinUAE with the config and check if you are able to see the harddrive with your source.

Install SAS/C and configure the amiga system.

### SetUp Visual Studio Code

Download VS Code from here: [Visual Studio Code](https://code.visualstudio.com/download)

### Install Visual Studio Code

Run the installer and follow the installation steps.

### Task Runner-Plugin

Next search in the marketplace for "Task Runner" and install it.

#### Configure Task Runner

Open the file .vscode/tasks.json.

Adapt the configuration:

- Path to your WinUA installation: C:/Development/Amiga/UAE/winuae64.exe
- name of the config file: config-micro.uae

You might need to restart VsCode to see the tasks on the left bottom under "Task Runner".
You should see:

- Start UAE
- Stop UAE
