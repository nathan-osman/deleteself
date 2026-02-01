## deleteself

This project contains a library that can be used by an application to delete itself after execution terminates. There are much simpler ways to do this (scheduled tasks, etc.) but the goal of this project is to provide a proof-of-concept on how this might be done in a perfectly clean way using Windows API functions / system calls — no external assistance required and nothing left behind.

### The Problem

Unlike POSIX operating systems, Windows does not allow files to be deleted when a handle to them is open. When the Windows program loader launches an executable file, the file's contents are mapped into the new process and therefore it cannot be deleted while the process is running. Executable files cannot directly delete themselves.

But...

### The Solution

Another process can wait for the executable file to terminate with [`WaitForSingleObject`](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject) and then delete it. But won't this "helper" executable then have the same issue — not being able to remove itself?

Yes, but not if we perform a clever trick.

When creating a new process with [`CreateProcessW`](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessw), there's a special flag we can pass: `CREATE_SUSPENDED`. This creates the process, maps the executable into memory, and performs a number of initialization steps for us, but crucially, it _does not start the process_. We can make a snapshot of the process's memory and then call an undocumented function in `ntdll.dll` called `NtUnmapViewOfSection`. This will unmap the executable file from the process's memory. Now if we then copy the snapshot back into the process's memory, we should (with a few caveats) be able to resume the process which is now no longer backed by a file on disk.

And now this "memory-only" process will wait for our original application to end at which point it will promptly delete the file and terminate itself.
