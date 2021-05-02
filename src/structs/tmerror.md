# TMError

This struct is used in a static table of errors at `0x004E8840` of size 32. The enum `ERROR_CODE` can be used to
index into it. `DisplayErrorMessage() @ 0x004BC660` indexes into the table to print to a
`MessageBox`.

```c
struct TMError {
  UINT msgBoxType;
  LPCSTR errorStrTitle;
  LPCSTR errorStrDescription;
};