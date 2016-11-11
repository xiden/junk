﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Runtime.InteropServices;

using element = System.Single;

namespace Jk {
	[XmlType("Jk.Vector2f")]
	[StructLayout(LayoutKind.Explicit, Pack = 4, Size = 8)]
	public struct Vector2f {
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

		public override bool Equals(object obj) {
			if (obj is Vector2f)
				return (Vector2f)obj == this;
			else
				return false;
		}

		public override int GetHashCode() {
			return (int)(X + Y);
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
			if (l == 0)
				return;
			else if (l == 1)
				return;
			l = (element)Math.Sqrt(l);
			X /= l;
			Y /= l;
		}

		public Vector2f Normalize() {
			Vector2f v = this;
			v.NormalizeSelf();
			return v;
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

		public Vector2f Saturate(element min, element max) {
			Vector2f v = this;
			v.SaturateSelf(min, max);
			return v;
		}

		public void SaturateSelf(Vector2f min, Vector2f max) {
			if (X < min.X)
				X = min.X;
			else if (max.X < X)
				X = max.X;
			if (Y < min.Y)
				Y = min.Y;
			else if (max.Y < Y)
				Y = max.Y;
		}

		public Vector2f Saturate(Vector2f min, Vector2f max) {
			Vector2f v = this;
			v.SaturateSelf(min, max);
			return v;
		}

		public void AbsSelf() {
			X = Math.Abs(X);
			Y = Math.Abs(Y);
		}

		public Vector2f Abs() {
			var v = this;
			v.AbsSelf();
			return v;
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

		static public bool operator ==(Vector2f v1, Vector2f v2) {
			return v1.X == v2.X && v1.Y == v2.Y;
		}

		static public bool operator !=(Vector2f v1, Vector2f v2) {
			return v1.X != v2.X || v1.Y != v2.Y;
		}

		static public Vector2f operator +(Vector2f v) {
			return v;
		}

		static public Vector2f operator -(Vector2f v) {
			return -v;
		}

		static public Vector2f operator +(Vector2f v1, Vector2f v2) {
			return new Vector2f(v1.X + v2.X, v1.Y + v2.Y);
		}

		static public Vector2f operator -(Vector2f v1, Vector2f v2) {
			return new Vector2f(v1.X - v2.X, v1.Y - v2.Y);
		}

		static public Vector2f operator *(Vector2f v, element s) {
			return new Vector2f(v.X * s, v.Y * s);
		}

		static public Vector2f operator /(Vector2f v, element s) {
			return new Vector2f(v.X / s, v.Y / s);
		}

		static public Vector2f operator *(Vector2f v1, Vector2f v2) {
			return new Vector2f(v1.X * v2.X, v1.Y * v2.Y);
		}

		static public Vector2f operator /(Vector2f v1, Vector2f v2) {
			return new Vector2f(v1.X / v2.X, v1.Y / v2.Y);
		}
	}
}
