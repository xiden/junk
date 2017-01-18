using System;
using System.Xml.Serialization;
using System.Runtime.InteropServices;

using element = System.Double;
using thisclass = Jk.Vector3d;

namespace Jk {
	[XmlType("Jk.Vector3d")]
	[StructLayout(LayoutKind.Explicit, Pack = 8, Size = 24)]
	public struct Vector3d {
		public static readonly thisclass Zero = new thisclass();
		public static readonly thisclass MinValue = new thisclass(element.MinValue, element.MinValue, element.MinValue);
		public static readonly thisclass MaxValue = new thisclass(element.MaxValue, element.MaxValue, element.MaxValue);

		[FieldOffset(0)]
		public element X;
		[FieldOffset(8)]
		public element Y;
		[FieldOffset(16)]
		public element Z;

		public Vector3d(element x, element y, element z) {
			X = x;
			Y = y;
			Z = z;
		}

		public Vector3d(Vector3i v) {
			X = (element)v.X;
			Y = (element)v.Y;
			Z = (element)v.Z;
		}

		public Vector3d(element[] arr) {
			X = arr[0];
			Y = arr[1];
			Z = arr[2];
		}

		public element this[int i] {
			get {
				switch (i) {
				case 0:
					return X;
				case 1:
					return Y;
				case 2:
					return Z;
				default:
					throw new IndexOutOfRangeException();
				}
			}
			set {
				switch (i) {
				case 0:
					X = value;
					break;
				case 1:
					Y = value;
					break;
				case 2:
					Z = value;
					break;
				default:
					throw new IndexOutOfRangeException();
				}
			}
		}

		public bool IsZero {
			get {
				return X == 0 && Y == 0 && Z == 0;
			}
		}

		public override bool Equals(object obj) {
			if (obj is thisclass)
				return (thisclass)obj == this;
			else
				return false;
		}

		public override int GetHashCode() {
			return X.GetHashCode() ^ Y.GetHashCode() << 2 ^ Z.GetHashCode() >> 2;
		}

		public override string ToString() {
			return string.Format("{{ {0}, {1}, {2} }}", X, Y, Z);
		}

		public element LengthSquare {
			get { return X * X + Y * Y + Z * Z; }
		}

		public element Length {
			get { return (element)Math.Sqrt(LengthSquare); }
		}

		public void NormalizeSelf() {
			element l = LengthSquare;
			if (l == 0 || l == 1)
				return;
			l = (element)Math.Sqrt(l);
			X /= l;
			Y /= l;
			Z /= l;
		}

		public thisclass Normalize() {
			element l = LengthSquare;
			if (l == 0 || l == 1)
				return this;
			l = (element)Math.Sqrt(l);
			return new thisclass(X / l, Y / l, Z / l);
		}

		public void SaturateSelf(element min, element max) {
			if (X < min)
				X = min;
			else if (max < X)
				X = max;
			if (Y < min)
				Y = min;
			else if (max < Y)
				Y = max;
			if (Z < min)
				Z = min;
			else if (max < Z)
				Z = max;
		}

		public thisclass Saturate(element min, element max) {
			var v = this;
			v.SaturateSelf(min, max);
			return v;
		}

		public void SaturateSelf(thisclass min, thisclass max) {
			if (X < min.X)
				X = min.X;
			else if (max.X < X)
				X = max.X;
			if (Y < min.Y)
				Y = min.Y;
			else if (max.Y < Y)
				Y = max.Y;
			if (Z < min.Z)
				Z = min.Z;
			else if (max.Z < Z)
				Z = max.Z;
		}

		public thisclass Saturate(thisclass min, thisclass max) {
			var v = this;
			v.Saturate(min, max);
			return v;
		}

		public void AbsSelf() {
			X = Math.Abs(X);
			Y = Math.Abs(Y);
			Z = Math.Abs(Z);
		}

		public thisclass Abs() {
			var v = this;
			v.AbsSelf();
			return v;
		}

		public element Sum() {
			return X + Y + Z;
		}

		public element Max {
			get {
				return Math.Max(X, Math.Max(Y, Z));
			}
		}

		public element Min {
			get {
				return Math.Min(X, Math.Min(Y, Z));
			}
		}

		public void ElementWiseMinSelf(thisclass v) {
			if (v.X < this.X) this.X = v.X;
			if (v.Y < this.Y) this.Y = v.Y;
			if (v.Z < this.Z) this.Z = v.Z;
		}

		public void ElementWiseMaxSelf(thisclass v) {
			if (this.X < v.X) this.X = v.X;
			if (this.Y < v.Y) this.Y = v.Y;
			if (this.Z < v.Z) this.Z = v.Z;
		}

		public element Dot(thisclass v) {
			return X * v.X + Y * v.Y + Z * v.Z;
		}

		static public bool operator ==(thisclass v1, thisclass v2) {
			return v1.X == v2.X && v1.Y == v2.Y && v1.Z == v2.Z;
		}

		static public bool operator !=(thisclass v1, thisclass v2) {
			return v1.X != v2.X || v1.Y != v2.Y || v1.Z != v2.Z;
		}

		static public bool operator <(thisclass v1, thisclass v2) {
			if (v1.X < v2.X) return true;
			if (v1.X > v2.X) return false;
			if (v1.Y < v2.Y) return true;
			if (v1.Y > v2.Y) return false;
			if (v1.Z < v2.Z) return true;
			if (v1.Z > v2.Z) return false;
			return false;
		}

		static public bool operator >(thisclass v1, thisclass v2) {
			if (v1.X > v2.X) return true;
			if (v1.X < v2.X) return false;
			if (v1.Y > v2.Y) return true;
			if (v1.Y < v2.Y) return false;
			if (v1.Z > v2.Z) return true;
			if (v1.Z < v2.Z) return false;
			return false;
		}

		static public thisclass operator +(thisclass v) {
			return v;
		}

		static public thisclass operator -(thisclass v) {
			return new thisclass(-v.X, -v.Y, -v.Z);
		}

		static public thisclass operator +(thisclass v1, thisclass v2) {
			return new thisclass(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
		}

		static public thisclass operator -(thisclass v1, thisclass v2) {
			return new thisclass(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z);
		}

		static public thisclass operator +(thisclass v, element s) {
			return new thisclass(v.X + s, v.Y + s, v.Z + s);
		}

		static public thisclass operator +(element s, thisclass v) {
			return new thisclass(s + v.X, s + v.Y, s + v.Z);
		}

		static public thisclass operator -(thisclass v, element s) {
			return new thisclass(v.X - s, v.Y - s, v.Z - s);
		}

		static public thisclass operator -(element s, thisclass v) {
			return new thisclass(s - v.X, s - v.Y, s - v.Z);
		}

		static public thisclass operator *(thisclass v, element s) {
			return new thisclass(v.X * s, v.Y * s, v.Z * s);
		}

		static public thisclass operator *(element s, thisclass v) {
			return new thisclass(v.X * s, v.Y * s, v.Z * s);
		}

		static public thisclass operator /(thisclass v, element s) {
			return new thisclass(v.X / s, v.Y / s, v.Z / s);
		}

		static public thisclass operator *(thisclass v1, thisclass v2) {
			return new thisclass(v1.X * v2.X, v1.Y * v2.Y, v1.Z * v2.Z);
		}

		static public thisclass operator /(thisclass v1, thisclass v2) {
			return new thisclass(v1.X / v2.X, v1.Y / v2.Y, v1.Z / v2.Z);
		}

		static public thisclass ElementWiseMin(thisclass v1, thisclass v2) {
			if (v2.X < v1.X) v1.X = v2.X;
			if (v2.Y < v1.Y) v1.Y = v2.Y;
			if (v2.Z < v1.Y) v1.Z = v2.Z;
			return v1;
		}

		static public thisclass ElementWiseMax(thisclass v1, thisclass v2) {
			if (v2.X < v1.X) v2.X = v1.X;
			if (v2.Y < v1.Y) v2.Y = v1.Y;
			if (v2.Z < v1.Z) v2.Z = v1.Z;
			return v2;
		}
	}
}
