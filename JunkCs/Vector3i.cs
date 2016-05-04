using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Runtime.InteropServices;

using element = System.Int32;

namespace Jk {
	[XmlType("Jk.Vector3i")]
	[StructLayout(LayoutKind.Explicit, Pack = 4, Size = 12)]
	public struct Vector3i {
		[FieldOffset(0)]
		public element X;
		[FieldOffset(4)]
		public element Y;
		[FieldOffset(8)]
		public element Z;

		public Vector3i(element x, element y, element z) {
			X = x;
			Y = y;
			Z = z;
		}

		public Vector3i(Vector3d v) {
			X = (element)v.X;
			Y = (element)v.Y;
			Z = (element)v.Z;
		}

		public Vector3i(element[] arr) {
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

		public override bool Equals(object obj) {
			if (obj is Vector3i)
				return (Vector3i)obj == this;
			else
				return false;
		}

		public override int GetHashCode() {
			return (int)(X + Y + Z);
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

		public void Normalize() {
			element l = LengthSquare;
			if (l == 0)
				return;
			else if (l == 1)
				return;
			l = (element)Math.Sqrt(l);
			X /= l;
			Y /= l;
			Z /= l;
		}

		public Vector3i GetNormalized() {
			Vector3i v = this;
			v.Normalize();
			return v;
		}

		public void Saturate(element min, element max) {
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

		public Vector3i GetSaturated(element min, element max) {
			Vector3i v = this;
			v.Saturate(min, max);
			return v;
		}

		public void Saturate(Vector3i min, Vector3i max) {
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

		public Vector3i GetSaturated(Vector3i min, Vector3i max) {
			Vector3i v = this;
			v.Saturate(min, max);
			return v;
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

		static public bool operator ==(Vector3i v1, Vector3i v2) {
			return v1.X == v2.X && v1.Y == v2.Y && v1.Z == v2.Z;
		}

		static public bool operator !=(Vector3i v1, Vector3i v2) {
			return v1.X != v2.X || v1.Y != v2.Y || v1.Z != v2.Z;
		}

		static public Vector3i operator +(Vector3i v) {
			return v;
		}

		static public Vector3i operator -(Vector3i v) {
			return -v;
		}

		static public Vector3i operator +(Vector3i v1, Vector3i v2) {
			return new Vector3i(v1.X + v2.X, v1.Y + v2.Y, v1.Z + v2.Z);
		}

		static public Vector3i operator -(Vector3i v1, Vector3i v2) {
			return new Vector3i(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z);
		}

		static public Vector3i operator *(Vector3i v, element s) {
			return new Vector3i(v.X * s, v.Y * s, v.Z * s);
		}

		static public Vector3i operator /(Vector3i v, element s) {
			return new Vector3i(v.X / s, v.Y / s, v.Z / s);
		}

		static public Vector3i operator *(Vector3i v1, Vector3i v2) {
			return new Vector3i(v1.X * v2.X, v1.Y * v2.Y, v1.Z * v2.Z);
		}

		static public Vector3i operator /(Vector3i v1, Vector3i v2) {
			return new Vector3i(v1.X / v2.X, v1.Y / v2.Y, v1.Z / v2.Z);
		}
	}
}
