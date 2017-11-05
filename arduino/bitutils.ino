// return the value (1 or 0) of bit n
unsigned int get_bit(byte b, unsigned int n)
{
  return (b >> n) & 1;
}

// set bit n (make it 1)
byte set_bit(byte b, unsigned int n)
{
  return b | (1 << n);
}

// unset bit n (make it 0)
byte unset_bit(byte b, unsigned int n)
{
  return b & ~(1 << n);
}

