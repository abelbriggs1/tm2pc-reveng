This struct is used in a static table of errors at `0x004E8840` of size 32. The enum `ERROR_CODE` can be used to
index into it. `DisplayErrorMessage() @ 0x004BC660` indexes into the table to print to a
`MessageBox`.

The value of `canRetry` seems to either be `0x30` or `0x35`. The values seem to be associated
with whether the `MessageBox` allows the user to retry an operation, usually with CD errors
(`ERR_CD_DRIVE_LOCKED`, `ERR_CD_DRIVE_ERROR`, `ERR_CD_INSERT_CD`).

```c
struct TMError {
  int canRetry; // ?
  char * errorStrTitle;
  char * errorStrDescription;
};