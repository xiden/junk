using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Runtime.InteropServices;

using element = System.Single;
using vector = Jk.Vector2f;
using thisclass = Jk.AABB2f;

namespace Jk {
	/// <summary>
	/// ２次元軸平行境界ボックス
	/// </summary>
	[XmlType("Jk.AABB2f")]
	[StructLayout(LayoutKind.Explicit, Pack = 4, Size = 16)]
	public struct AABB2f {
		[FieldOffset(0)]
		public vector Min;
		[FieldOffset(8)]
		public vector Max;

		public AABB2f(vector position) {
			this.Min = position;
			this.Max = position;
		}

		public AABB2f(vector min, vector max) {
			this.Min = min;
			this.Max = max;
		}

		public AABB2f(vector min, vector max, bool normalize) {
			if (normalize) {
				if (max.X <= min.X) {
					var t = min.X;
					min.X = max.X;
					max.X = t;
				}
				if (max.Y <= min.Y) {
					var t = min.Y;
					min.Y = max.Y;
					max.Y = t;
				}
			}
			this.Min = min;
			this.Max = max;
		}

		public AABB2f(IEnumerable<vector> positions) {
			bool first = true;
			vector min = new vector(), max = new vector();
			foreach(var p in positions) {
				if (first) {
					first = false;
					min = p;
					max = p;
				} else {
					min.ElementWiseMinSelf(p);
					max.ElementWiseMaxSelf(p);
				}
			}
			this.Min = min;
			this.Max = max;
		}

		public override bool Equals(object obj) {
			if (obj is thisclass)
				return (thisclass)obj == this;
			else
				return false;
		}

		public override int GetHashCode() {
			return Min.GetHashCode() ^ Max.GetHashCode() << 1;
		}

		public override string ToString() {
			return string.Format("{{ {0}, {1} }}", Min, Max);
		}

		public bool IsValid {
			get {
				return Min.X <= Max.X && Min.Y <= Max.Y;
			}
		}

		public vector Size {
			get {
				return Max - Min;
			}
		}

		public vector Center {
			get {
				return (Min + Max) / 2;
			}
		}

		public vector Extents {
			get {
				return (Max - Min) / 2;
			}
		}

		public element Perimeter {
			get {
				var size = Max - Min;
				return 2 * (size.X + size.Y);
			}
		}

		public element VolumeAndEdgesLength {
			get {
				var s = this.Size;
				return s.X * s.Y + s.X + s.Y;
			}
		}

		public bool Contains(thisclass aabb) {
			return Min.X <= aabb.Min.X && Min.Y <= aabb.Min.Y && aabb.Max.X <= Max.X && aabb.Max.Y <= Max.Y;
		}

		public bool Intersects(thisclass aabb) {
			if (this.Max.X < aabb.Min.X || aabb.Max.X < this.Min.X)
				return false;
			if (this.Max.Y < aabb.Min.Y || aabb.Max.Y < this.Min.Y)
				return false;
			return true;
		}

		public thisclass Merge(vector v) {
			return new thisclass(vector.ElementWiseMin(this.Min, v), vector.ElementWiseMax(this.Max, v));
		}

		public thisclass Merge(thisclass aabb) {
			return new thisclass(vector.ElementWiseMin(this.Min, aabb.Min), vector.ElementWiseMax(this.Max, aabb.Max));
		}

		public void MergeSelf(vector v) {
			this.Min.ElementWiseMinSelf(v);
			this.Max.ElementWiseMaxSelf(v);
		}

		public void MergeSelf(thisclass aabb) {
			this.Min.ElementWiseMinSelf(aabb.Min);
			this.Max.ElementWiseMaxSelf(aabb.Max);
		}

		public thisclass Expand(element s) {
			return new thisclass(this.Min - s, this.Max + s);
		}

		public thisclass Expand(vector v) {
			return new thisclass(this.Min - v, this.Max + v);
		}

		static public bool operator ==(thisclass b1, thisclass b2) {
			return b1.Min == b2.Min && b1.Max == b2.Max;
		}

		static public bool operator !=(thisclass b1, thisclass b2) {
			return b1.Min != b2.Min || b1.Max != b2.Max;
		}

		static public thisclass operator +(thisclass b, vector v) {
			return new thisclass(b.Min + v, b.Max + v);
		}

		static public thisclass operator +(vector v, thisclass b) {
			return new thisclass(b.Min + v, b.Max + v);
		}

		static public thisclass operator -(thisclass b, vector v) {
			return new thisclass(b.Min - v, b.Max - v);
		}

		static public thisclass operator *(thisclass b, element s) {
			return new thisclass(b.Min * s, b.Max * s);
		}

		static public thisclass operator /(thisclass b, element s) {
			return new thisclass(b.Min / s, b.Max / s);
		}

		static public thisclass operator *(thisclass b, vector v) {
			return new thisclass(b.Min * v, b.Max * v);
		}

		static public thisclass operator *(vector v, thisclass b) {
			return new thisclass(b.Min * v, b.Max * v);
		}

		static public thisclass operator /(thisclass b, vector v) {
			return new thisclass(b.Min / v, b.Max / v);
		}
	}
}
