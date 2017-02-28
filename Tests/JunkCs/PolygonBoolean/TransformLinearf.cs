﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.Runtime.InteropServices;

using element = System.Single;

namespace Jk {
	/// <summary>
	/// １次元リニア座標変換構造体
	/// </summary>
	[XmlType("Jk.TransformLinearf")]
	public struct TransformLinearf {
		/// <summary>
		/// スケーリング値
		/// </summary>
		public element Scale;

		/// <summary>
		/// スケーリング後の移動量
		/// </summary>
		public element Translate;

		/// <summary>
		/// コンストラクタ、縮尺と移動量を指定して初期化する
		/// </summary>
		/// <param name="scale">縮尺</param>
		/// <param name="translate">移動量</param>
		public TransformLinearf(element scale, element translate) {
			if (scale == 0.0) {
				StringBuilder sb = new StringBuilder();
				sb.AppendLine("TransformLineard.TransformLineard(element scale, element translate) に渡された引数が無効です。");
				sb.AppendLine("スケーリング値が 0 です。");
				sb.AppendFormat("scale={0}\n", scale);
				throw new Exception(sb.ToString());
			}
			this.Scale = scale;
			this.Translate = translate;
		}

		/// <summary>
		/// コンストラクタ、変換前と変換後の範囲を指定して初期化する
		/// </summary>
		/// <param name="rangeBefore">変換前範囲</param>
		/// <param name="rangeAfter">変換後範囲</param>
		public TransformLinearf(Rangef rangeBefore, Rangef rangeAfter) {
			element sizeb = rangeBefore.Size;
			element sizea = rangeAfter.Size;
			if (sizea == 0.0 || sizeb == 0.0) {
				StringBuilder sb = new StringBuilder();
				sb.AppendLine("TransformLineard.TransformLineard(Ranged rangeBefore, Ranged rangeAfter) に渡された引数が無効です。");
				sb.AppendLine("変換前範囲または変換後範囲のサイズが 0 です。");
				sb.AppendFormat("rangeBefore.Size={0}\n", sizeb);
				sb.AppendFormat("rangeAfter.Size={0}\n", sizea);
				throw new Exception(sb.ToString());
			}
			this.Scale = sizea / sizeb;
			this.Translate = rangeAfter.V1 - rangeBefore.V1 * this.Scale;
		}

		/// <summary>
		/// 座標変換を行う
		/// </summary>
		/// <param name="val">変換する値</param>
		/// <returns>変換後の値</returns>
		public element Cnv(element val) {
			return val * this.Scale + this.Translate;
		}

		/// <summary>
		/// 逆座標変換を行う
		/// </summary>
		/// <param name="val">変換する値</param>
		/// <returns>変換後の値</returns>
		public element InvCnv(element val) {
			return (val - this.Translate) / this.Scale;
		}

		/// <summary>
		///	座標変換を合成する
		/// </summary>
		/// <param name="transform">１次元座標変換構造体</param>
		/// <returns>合成された１次元座標変換構造体、 this で変換後に transform で変換を行う座標変換構造体</returns>
		public TransformLinearf Multiply(TransformLinearf transform) {
			return new TransformLinearf(this.Scale * transform.Scale, this.Translate * transform.Scale + transform.Translate);
		}

		/// <summary>
		/// 逆変換を作成する
		/// </summary>
		/// <returns>逆変換</returns>
		public TransformLinearf Invert() {
			return new TransformLinearf(1 / this.Scale, -this.Translate / this.Scale);
		}

		/// <summary>
		/// 文字列表現を取得
		/// </summary>
		/// <returns>文字列</returns>
		public override string ToString() {
			return string.Format("{{ Scale={0}, Translate={1} }}", this.Scale, this.Translate);
		}
	}
}
