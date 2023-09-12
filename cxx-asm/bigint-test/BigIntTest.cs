using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;

namespace bigint_test
{
	internal class MyBigInt : IDisposable
	{
		private IntPtr _bigIntHandle;

		internal MyBigInt(Span<byte> data)
		{
			unsafe
			{
				fixed (byte* ptr = data)
				{
					_bigIntHandle = BigInt_CreateFromBytes(ptr, data.Length);
				}
			}
			if (_bigIntHandle == IntPtr.Zero)
			{
				throw new Exception("BigInt creation failed.");
			}
		}

		private void ReleaseUnmanagedResources()
		{
			BigInt_Release(_bigIntHandle);
			_bigIntHandle = IntPtr.Zero;
		}

		public void Dispose()
		{
			ReleaseUnmanagedResources();
			GC.SuppressFinalize(this);
		}

		~MyBigInt()
		{
			ReleaseUnmanagedResources();
		}

		public override string ToString()
		{
			if (_bigIntHandle == IntPtr.Zero)
			{
				return "";
			}
			var expectedLength = 100;
			var res = new StringBuilder(100);
			var actualLength = BigInt_ToString(_bigIntHandle, res, res.Capacity);
			while (actualLength < 0)
			{
				expectedLength = res.EnsureCapacity(expectedLength * 2);
				actualLength = BigInt_ToString(_bigIntHandle, res, res.Capacity);
			}

			return res.ToString(0, actualLength);
		}

		[DllImport("bigint.dll")]
		private static extern unsafe IntPtr BigInt_CreateFromBytes(byte* data, int size);

		[DllImport("bigint.dll")]
		private static extern void BigInt_Release(IntPtr handle);

		[DllImport("bigint.dll")]
		private static extern int BigInt_ToString(IntPtr handle, StringBuilder str, int length);
	}

	[TestClass]
	public class BigIntTest
	{
		[TestMethod]
		public void GivenRandomBytes_WhenCreateFromBytes_ThenStringRepresentationsEqual()
		{
			var random = new Random();
			for (int i = 1; i < 512; i++)
			{
				var data = new byte[i+1];
				var actualBytes = new Span<byte>(data, 0, data.Length - 1);
				for (int j = 0; j < 1000; j++)
				{
					random.NextBytes(actualBytes);
					var gt = new BigInteger(data);
					using var ut = new MyBigInt(actualBytes);
					Assert.AreEqual(gt.ToString(), ut.ToString(), $"Size: {i}, Bytes: {string.Join(", ", actualBytes.ToArray())}");
				}
			}
		}
	}
}
