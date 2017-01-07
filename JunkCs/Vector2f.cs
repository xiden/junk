using System;
using System.Xml.Serialization;
using System.Runtime.InteropServices;

using element = System.Single;
using thisclass = Jk.Vector2f;

namespace Jk {
	[XmlType("Jk.Vector2f")]
	[StructLayout(LayoutKind.Explicit, Pack = 4, Size = 8)]
	public struct Vector2f {
		public static readonly thisclass Zero = new thisclass();

		[FieldOffset(0)]
		public element X;
		[FieldOffset(4)]
		public element Y;

		public Vector2f(element x, element y) {
			X = x;
			Y = y;
		}

		public Vector2f(Vector2i v) {
			X = (element)v.X;
			Y = (element)v.Y;
		}

		public Vector2f(element[] arr) {
			X = arr[0];
			Y = arr[1];
		}

		public element this[int i] {
			get {
				switch (i) {
				case 0:
					return X;
				case 1:
					return Y;
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
				default:
					throw new IndexOutOfRangeException();
				}
			}
		}

		public bool IsZero {
			get {
				return X == 0 && Y == 0;
			}
		}

		public override bool Equals(object obj) {
			if (obj is thisclass)
				return (thisclass)obj == this;
			else
				return false;
		}

		public override int GetHashCode() {
			return X.GetHashCode() ^ Y.GetHashCode() << 2;
		}

		public override string ToString() {
			return string.Format("{{ {0}, {1} }}", X, Y);
		}

		public string ToString(string f) {
			return string.Format("{{ {0:" + f + "}, {1:" + f + "} }}", X, Y);
		}

		public element LengthSquare {
			get { return X * X + Y * Y; }
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
		}

		public thisclass Normalize() {
			element l = LengthSquare;
			if (l == 0 || l == 1)
				return this;
			l = (element)Math.Sqrt(l);
			return new thisclass(X / l, Y / l);
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
		}

		public thisclass Saturate(element min, element max) {
			thisclass v = this;
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
		}

		public thisclass Saturate(thisclass min, thisclass max) {
			thisclass v = this;
			v.SaturateSelf(min, max);
			return v;
		}

		public void AbsSelf() {
			X = Math.Abs(X);
			Y = Math.Abs(Y);
		}

		public thisclass Abs() {
			var v = this;
			v.AbsSelf();
			return v;
		}

		public void VerticalCwSelf() {
			var t = X;
			X = -Y;
			Y = t;
		}

		public thisclass VerticalCw() {
			return new thisclass(-Y, X);
		}

		public element Sum() {
			return X + Y;
		}

		public element Max {
			get {
				return Math.Max(X, Y);
			}
		}

		public element Min {
			get {
				return Math.Min(X, Y);
			}
		}

		public void ElementWiseMinSelf(thisclass v) {
			if (v.X < this.X) this.X = v.X;
			if (v.Y < this.Y) this.Y = v.Y;
		}

		public void ElementWiseMaxSelf(thisclass v) {
			if (this.X < v.X) this.X = v.X;
			if (this.Y < v.Y) this.Y = v.Y;
		}

		public element Dot(thisclass v) {
			return X * v.X + Y * v.Y;
		}

		static public bool operator ==(thisclass v1, thisclass v2) {
			return v1.X == v2.X && v1.Y == v2.Y;
		}

		static public bool operator !=(thisclass v1, thisclass v2) {
			return v1.X != v2.X || v1.Y != v2.Y;
		}

		static public bool operator <(thisclass v1, thisclass v2) {
			if (v1.X < v2.X) return true;
			if (v1.X > v2.X) return false;
			if (v1.Y < v2.Y) return true;
			if (v1.Y > v2.Y) return false;
			return false;
		}

		static public bool operator >(thisclass v1, thisclass v2) {
			if (v1.X > v2.X) return true;
			if (v1.X < v2.X) return false;
			if (v1.Y > v2.Y) return true;
			if (v1.Y < v2.Y) return false;
			return false;
		}

		static public thisclass operator +(thisclass v) {
			return v;
		}

		static public thisclass operator -(thisclass v) {
			return new thisclass(-v.X, -v.Y);
		}

		static public thisclass operator +(thisclass v1, thisclass v2) {
			return new thisclass(v1.X + v2.X, v1.Y + v2.Y);
		}

		static public thisclass operator -(thisclass v1, thisclass v2) {
			return new thisclass(v1.X - v2.X, v1.Y - v2.Y);
		}

		static public thisclass operator +(thisclass v, element s) {
			return new thisclass(v.X + s, v.Y + s);
		}

		static public thisclass operator +(element s, thisclass v) {
			return new thisclass(s + v.X, s + v.Y);
		}

		static public thisclass operator -(thisclass v, element s) {
			return new thisclass(v.X - s, v.Y - s);
		}

		static public thisclass operator -(element s, thisclass v) {
			return new thisclass(s - v.X, s - v.Y);
		}


		static public thisclass operator *(thisclass v, element s) {
			return new thisclass(v.X * s, v.Y * s);
		}

		static public thisclass operator *(element s, thisclass v) {
			return new thisclass(v.X * s, v.Y * s);
		}

		static public thisclass operator /(thisclass v, element s) {
			return new thisclass(v.X / s, v.Y / s);
		}

		static public thisclass operator *(thisclass v1, thisclass v2) {
			return new thisclass(v1.X * v2.X, v1.Y * v2.Y);
		}

		static public thisclass operator /(thisclass v1, thisclass v2) {
			return new thisclass(v1.X / v2.X, v1.Y / v2.Y);
		}

		static public thisclass ElementWiseMin(thisclass v1, thisclass v2) {
			if (v2.X < v1.X) v1.X = v2.X;
			if (v2.Y < v1.Y) v1.Y = v2.Y;
			return v1;
		}

		static public thisclass ElementWiseMax(thisclass v1, thisclass v2) {
			if (v2.X < v1.X) v2.X = v1.X;
			if (v2.Y < v1.Y) v2.Y = v1.Y;
			return v2;
		}


#if UNITY_5
		public Vector2f(UnityEngine.Vector2 v) {
			X = (element)v.x;
			Y = (element)v.y;
		}

		public static implicit operator thisclass(UnityEngine.Vector2 v) {
			return new thisclass(v.x, v.y);
		}
		public static implicit operator thisclass(UnityEngine.Vector3 v) {
			return new thisclass(v.x, v.y);
		}
		public static implicit operator UnityEngine.Vector2(thisclass v) {
			return new UnityEngine.Vector2(v.X, v.Y);
		}
		public static implicit operator UnityEngine.Vector3(thisclass v) {
			return new UnityEngine.Vector3(v.X, v.Y, 0);
		}
#endif
	}
}
