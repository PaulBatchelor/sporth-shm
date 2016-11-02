(
_shm "./shm_client.so" fl

_test 5678 _shm fe

_test get "val" print drop

_shm fc

0 0
)
