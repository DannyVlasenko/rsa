using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.VisualBasic.CompilerServices;

namespace bigint_test
{
	internal class MyBigInt : IDisposable, IComparable<MyBigInt>, IComparable, IEquatable<MyBigInt>
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

		public int CompareTo(MyBigInt? other)
		{
			if (ReferenceEquals(null, other)) return 1;
			if (ReferenceEquals(this, other)) return 0;
			return BigInt_Compare(_bigIntHandle, other._bigIntHandle);
		}

		public int CompareTo(object? obj)
		{
			if (ReferenceEquals(null, obj)) return 1;
			if (ReferenceEquals(this, obj)) return 0;
			return obj is MyBigInt other ? CompareTo(other) : throw new ArgumentException($"Object must be of type {nameof(MyBigInt)}");
		}

		public static bool operator <(MyBigInt? left, MyBigInt? right)
		{
			return Comparer<MyBigInt>.Default.Compare(left, right) < 0;
		}

		public static bool operator >(MyBigInt? left, MyBigInt? right)
		{
			return Comparer<MyBigInt>.Default.Compare(left, right) > 0;
		}

		public static bool operator <=(MyBigInt? left, MyBigInt? right)
		{
			return Comparer<MyBigInt>.Default.Compare(left, right) <= 0;
		}

		public static bool operator >=(MyBigInt? left, MyBigInt? right)
		{
			return Comparer<MyBigInt>.Default.Compare(left, right) >= 0;
		}

		public bool Equals(MyBigInt? other)
		{
			if (ReferenceEquals(null, other)) return false;
			if (ReferenceEquals(this, other)) return true;
			return BigInt_Equals(_bigIntHandle, other._bigIntHandle);
		}

		public override bool Equals(object? obj)
		{
			if (ReferenceEquals(null, obj)) return false;
			if (ReferenceEquals(this, obj)) return true;
			if (obj.GetType() != this.GetType()) return false;
			return Equals((MyBigInt)obj);
		}

		public override int GetHashCode()
		{
			// MSDN does not require an object to be immutable
			// ReSharper disable once NonReadonlyMemberInGetHashCode
			return BigInt_Hash(_bigIntHandle);
		}

		public static bool operator ==(MyBigInt? left, MyBigInt? right)
		{
			return Equals(left, right);
		}

		public static bool operator !=(MyBigInt? left, MyBigInt? right)
		{
			return !Equals(left, right);
		}

		[DllImport("bigint.dll")]
		private static extern unsafe IntPtr BigInt_CreateFromBytes(byte* data, int size);

		[DllImport("bigint.dll")]
		private static extern void BigInt_Release(IntPtr handle);

		[DllImport("bigint.dll")]
		private static extern int BigInt_Hash(IntPtr handle);

		[DllImport("bigint.dll")]
		private static extern int BigInt_ToString(IntPtr handle, StringBuilder str, int length);

		[DllImport("bigint.dll")]
		private static extern bool BigInt_Equals(IntPtr lhs, IntPtr rhs);

		[DllImport("bigint.dll")]
		private static extern int BigInt_Compare(IntPtr lhs, IntPtr rhs);
	}

	[TestClass]
	public class BigIntTest
	{
		[TestMethod]
		public void GivenRandomBytes_WhenCreateFromBytes_ThenStringRepresentationsEqual()
		{
			var random = new Random();
			for (int i = 1; i < 6; i++)
			{
				var data = new byte[i+1];
				var actualBytes = new Span<byte>(data, 0, data.Length - 1);
				for (int j = 0; j < 100; j++)
				{
					random.NextBytes(actualBytes);
					var gt = new BigInteger(data);
					using var ut = new MyBigInt(actualBytes);
					Assert.AreEqual(gt.ToString(), ut.ToString(), $"Size: {i}, Bytes: {string.Join(", ", actualBytes.ToArray())}");
				}
			}
		}
		[TestMethod]
		public void GivenRandomBytes_WhenCreateFromBytes_ThenComparisonsGiveSameResults()
		{
			var random = new Random();
			for (int i = 1; i < 512; i++)
			{
				var data = new byte[i+1];
				var actualBytes = new Span<byte>(data, 0, data.Length - 1);
				for (int j = 0; j < 1000; j++)
				{
					random.NextBytes(actualBytes);
					var gt1 = new BigInteger(data);
					using var ut1 = new MyBigInt(actualBytes);
					random.NextBytes(actualBytes);
					var gt2 = new BigInteger(data);
					using var ut2 = new MyBigInt(actualBytes);
					Assert.AreEqual(gt1 < gt2, ut1 < ut2);
					Assert.AreEqual(gt1 <= gt2, ut1 <= ut2);
					Assert.AreEqual(gt1 > gt2, ut1 > ut2);
					Assert.AreEqual(gt1 >= gt2, ut1 >= ut2);
					Assert.AreEqual(gt1 == gt2, ut1 == ut2);
					Assert.AreEqual(gt1 != gt2, ut1 != ut2);
				}
			}
		}
	}
}
