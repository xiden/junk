using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Jk;

namespace PolygonBoolean {
	public struct Tf {
		public TransformLinearf X;
		public TransformLinearf Y;

		public Vector2f Fw(Vector2f v) {
			return new Vector2f(X.Cnv(v.X), Y.Cnv(v.Y));
		}

		public Vector2f Bw(Vector2f v) {
			return new Vector2f(X.InvCnv(v.X), Y.InvCnv(v.Y));
		}

		public Tf Mul(Tf tf) {
			return new Tf {
				X = this.X.Multiply(tf.X),
				Y = this.Y.Multiply(tf.Y),
			};
		}

		public Tf Invert() {
			return new Tf {
				X = this.X.Invert(),
				Y = this.Y.Invert(),
			};
		}
	}
}
