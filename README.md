### Data Longevity

All data returned by libikarus is ephemeral and only represents the state of the project at the time of the request.
A snapshot if you will.
One must not rely on it representing the actual state of the project at any given time. The data is simply copied
from the underlying data sources and returned to the caller.

No mechanisms are provided to avoid race conditions. LibIkarus itself should only be used in a single-threaded context.
However, nothing breaks if you do use it in a multithreaded context, that is, libikarus is threadsafe.
You just cannot rely on the data being consistent.
This goes especially for inter-process access to the same project.