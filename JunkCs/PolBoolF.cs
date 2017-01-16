using System;
using System.Linq;
using System.Collections.Generic;

using element = System.Single;
using vector = Jk.Vector2f;
using volume = Jk.AABB2f;

namespace Jk {
	/// <summary>
	/// ポリゴンのブーリアン演算を行うクラス
	/// </summary>
	public class PolBoolF {
		#region クラス
		/// <summary>
		/// ポリゴンの頂点
		/// </summary>
		public struct Vertex {
			/// <summary>
			/// 位置
			/// </summary>
			public vector Position;

			/// <summary>
			/// ユーザーデータ
			/// </summary>
			public object UserData;

			/// <summary>
			/// コンストラクタ、位置を指定して初期化する
			/// </summary>
			/// <param name="position">位置</param>
			public Vertex(vector position) {
				this.Position = position;
				this.UserData = null;
			}

			/// <summary>
			/// コンストラクタ、位置とユーザーデータを指定して初期化する
			/// </summary>
			/// <param name="position">位置</param>
			/// <param name="userData">ユーザーデータ</param>
			public Vertex(vector position, object userData) {
				this.Position = position;
				this.UserData = userData;
			}
		}

		/// <summary>
		/// ポリゴン情報、頂点は３つ以上で並びは時計回り且つ自己交差してはならない
		/// </summary>
		public class Polygon {
			/// <summary>
			/// 頂点配列
			/// </summary>
			public List<Vertex> Vertices;

			/// <summary>
			/// エッジのユーザーデータ配列、null ならエッジユーザーデータ無し
			/// </summary>
			public List<object> EdgesUserData;

			/// <summary>
			/// 穴配列、null なら穴無し
			/// </summary>
			public List<Hole> Holes;

			/// <summary>
			/// このポリゴンに紐づくユーザーデータ
			/// </summary>
			public object UserData;

			/// <summary>
			/// ポリゴンの境界ボリューム
			/// </summary>
			public volume Volume;

			/// <summary>
			/// ポリゴンの面積
			/// </summary>
			public element Area;

			/// <summary>
			/// 時計回りかどうか
			/// </summary>
			public bool CW;

			public PolBoolF Owner;
			public int GroupIndex;
			public int PolygonIndex;

			/// <summary>
			/// コンストラクタ、頂点座標配列、エッジユーザーデータ配列、穴配列を渡して初期化する
			/// </summary>
			/// <param name="vertices">頂点配列</param>
			/// <param name="edgesUserData">エッジのユーザーデータ配列、null 指定可能</param>
			/// <param name="holes">穴配列、null 指定可能</param>
			public Polygon(List<Vertex> vertices, List<object> edgesUserData, List<Hole> holes) {
				this.Vertices = vertices;
				this.EdgesUserData = edgesUserData;
				this.Holes = holes;
			}

			/// <summary>
			/// Validation() メソッドの準備処理
			/// </summary>
			/// <param name="lfinder">線分検索用オブジェクト</param>
			/// <param name="epsilon">頂点半径</param>
			/// <returns>使えるなら true が返る</returns>
			public ValidationResult PrepareValidation(LineFinder lfinder, element epsilon) {
				var vertices = this.Vertices;
				var nvts = vertices.Count;
				var holes = this.Holes;
				element area;

				// 最低でも３点以上ないとだめ
				if (nvts < 3)
					return new ValidationResult(false, "ポリゴンの頂点数は３以上でなければなりません。");

				// 頂点数とエッジ数が矛盾してたらだめ
				if (this.EdgesUserData != null && this.EdgesUserData.Count != nvts)
					return new ValidationResult(false, "ポリゴンの頂点数とエッジ数が矛盾しています。");

				// 時計回りチェック
				area = Area(vertices);
				if (0 <= area)
					return new ValidationResult(false, "ポリゴンは時計回りでなければなりません。");

				if (holes != null) {
					for (int i = holes.Count - 1; i != -1; i--) {
						var hole = holes[i];

						// 最低でも３点以上ないとだめ
						if (hole.Vertices.Count < 3)
							return new ValidationResult(false, "穴の頂点数は３以上でなければなりません。");

						// 頂点数とエッジ数が矛盾してたらだめ
						if (hole.EdgesUserData != null && hole.EdgesUserData.Count != hole.Vertices.Count)
							return new ValidationResult(false, "穴の頂点数とエッジ数が矛盾しています。");

						// 時計回りチェック
						area = Area(hole.Vertices);
						if (area <= 0)
							return new ValidationResult(false, "穴は反時計回りでなければなりません。");
					}
				}

				// 全頂点とラインを検索用ツリーに登録
				int startId = 0;
				lfinder.Add(this, startId, vertices, epsilon);
				startId += vertices.Count;

				if (holes != null) {
					for (int i = 0, n = holes.Count; i < n; i++) {
						var hole = holes[i];
						lfinder.Add(this, startId, hole.Vertices, epsilon);
						startId += hole.Vertices.Count;
					}
				}

				return new ValidationResult(true, null);
			}

			/// <summary>
			/// ポリゴンがブーリアン演算の入力として使えるか調べる
			/// </summary>
			/// <param name="lfinder">線分検索用オブジェクト</param>
			/// <param name="epsilon">頂点半径</param>
			/// <returns>使えるなら true が返る</returns>
			public ValidationResult Validation(LineFinder lfinder, element epsilon) {
				var vertices = this.Vertices;
				var nvts = vertices.Count;
				var holes = this.Holes;

				// 辺同士の共有と交差チェック
				var epsilon2 = epsilon * epsilon;
				var p1 = vertices[0].Position;
				for (int i = 1; i <= nvts; i++) {
					var id1 = i - 1;
					var id2 = (id1 + 1) % nvts;
					var id3 = (id1 - 1 + nvts) % nvts;
					var p2 = vertices[i % nvts].Position;

					if(lfinder.TestShare(p1, p2, this, id1, epsilon2))
						return new ValidationResult(false, "ポリゴンは辺を共有してはなりません。");

					if (lfinder.TestIntersect(p1, p2, this, id1, id2, id3))
						return new ValidationResult(false, "ポリゴンは自己交差があってはなりません。");

					p1 = p2;
				}

				// 穴のチェック
				if (holes != null) {
					int startId = nvts;
					for (int i = 0, nholes = holes.Count; i < nholes; i++) {
						var hole = holes[i];
						var holevts = hole.Vertices;
						var nholevts = holevts.Count;

						// 辺同士の共有と交差チェック
						p1 = holevts[0].Position;

						for (int j = 1; j <= nholevts; j++) {
							var id1 = j - 1;
							var id2 = startId + (id1 + 1) % nholevts;
							var id3 = startId + (id1 - 1 + nholevts) % nholevts;
							var p2 = holevts[j % nholevts].Position;

							id1 += startId;

							if (lfinder.TestShare(p1, p2, this, id1, epsilon2))
								return new ValidationResult(false, "穴は辺を共有してはなりません。");

							if (lfinder.TestIntersect(p1, p2, this, id1, id2, id3))
								return new ValidationResult(false, "穴はポリゴンと交差したりは自己交差があってはなりません。");

							p1 = p2;
						}

						startId += nholevts;

						// 外側のポリゴン外に出ていないかチェック
						if (!PointInPolygon2(holevts[0].Position, vertices, true)) {
							return new ValidationResult(false, "穴はポリゴン外に出てはなりません。");
						}

						// TODO: 他の穴の中に入ってないかチェック
					}
				}

				return new ValidationResult(true, "有効なポリゴンです。ブーリアン処理に使用できます。");
			}

			/// <summary>
			/// 複製を作成
			/// </summary>
			/// <returns>複製</returns>
			public Polygon Clone() {
				var p = this.MemberwiseClone() as Polygon;
				if (p.Vertices != null)
					p.Vertices = new List<Vertex>(p.Vertices);
				if (p.EdgesUserData != null)
					p.EdgesUserData = new List<object>(p.EdgesUserData);
				var holes2 = p.Holes;
				if (holes2 != null) {
					var holes1 = this.Holes;
					p.Holes = holes2 = new List<Hole>(holes1);
					for (int i = holes2.Count - 1; i != -1; i--) {
						holes2[i] = holes1[i].Clone();
					}
				}
				return p;
			}

			/// <summary>
			/// ポリゴンを平行移動する
			/// </summary>
			/// <param name="offset">移動量</param>
			public void Offset(vector offset) {
				var vertices = this.Vertices;
				for (int i = vertices.Count - 1; i != -1; i--) {
					var v = vertices[i];
					v.Position += offset;
					vertices[i] = v;
				}
				var holes = this.Holes;
				if (holes != null) {
					for (int ihole = holes.Count - 1; ihole != -1; ihole--) {
						vertices = holes[ihole].Vertices;
						for (int i = vertices.Count - 1; i != -1; i--) {
							var v = vertices[i];
							v.Position += offset;
							vertices[i] = v;
						}
					}
				}
			}
		}

		/// <summary>
		/// ポリゴンの穴情報、頂点は３つ以上で並びは反時計回り且つ自己交差してはならない
		/// </summary>
		public class Hole {
			/// <summary>
			/// 頂点配列
			/// </summary>
			public List<Vertex> Vertices;

			/// <summary>
			/// エッジのユーザーデータ配列、null ならエッジユーザーデータ無し
			/// </summary>
			public List<object> EdgesUserData;

			/// <summary>
			/// コンストラクタ、頂点座標配列、エッジユーザーデータ配列を渡して初期化する
			/// </summary>
			/// <param name="vertices">頂点配列</param>
			public Hole(List<Vertex> vertices, List<object> edgesUserData) {
				this.Vertices = vertices;
				this.EdgesUserData = edgesUserData;
			}

			/// <summary>
			/// 複製を作成
			/// </summary>
			/// <returns>複製</returns>
			public Hole Clone() {
				var p = this.MemberwiseClone() as Hole;
				if (p.Vertices != null)
					p.Vertices = new List<Vertex>(p.Vertices);
				if (p.EdgesUserData != null)
					p.EdgesUserData = new List<object>(p.EdgesUserData);
				return p;
			}
		}

		/// <summary>
		/// ノードに付与するフラグ
		/// </summary>
		[Flags]
		public enum NodeFlags {
			/// <summary>
			/// ポリゴン内外が変化する切り替え部分のノードである
			/// </summary>
			InsideOutside = 1 << 0,

			/// <summary>
			/// エッジ上に挿入されるノード
			/// </summary>
			OnEdge = 1 << 1,
		}

		/// <summary>
		/// エッジに付与するフラグ
		/// </summary>
		[Flags]
		public enum EdgeFlags {
			/// <summary>
			/// 右側がポリゴン化済み
			/// </summary>
			RightPolygonized = 1 << 0,

			/// <summary>
			/// 左側がポリゴン化済み
			/// </summary>
			LeftPolygonized = 1 << 1,

			/// <summary>
			/// 右側が摘出済み
			/// </summary>
			RightRemoved = 1 << 2,

			/// <summary>
			/// 左側が摘出済み
			/// </summary>
			LeftRemoved = 1 << 3,
		}

		/// <summary>
		/// エッジへのノード挿入情報
		/// </summary>
		public struct NodeInsertion {
			/// <summary>
			/// エッジの線分パラメータ
			/// </summary>
			public element Parameter;

			/// <summary>
			/// 挿入するノード
			/// </summary>
			public Node Node;

			public NodeInsertion(element parameter, Node node) {
				this.Parameter = parameter;
				this.Node = node;
			}
		}

		/// <summary>
		/// ノード、多角形の頂点と多角形の交点部分に作成される
		/// </summary>
		public class Node {
			/// <summary>
			/// ノードの情報フラグ
			/// </summary>
			public NodeFlags Flags;

			/// <summary>
			/// ユニークなエッジインデックス、 NodeManager 内で同じ値があってはならない
			/// </summary>
			public uint UniqueIndex;

			/// <summary>
			/// 座標
			/// </summary>
			public vector Position;

			/// <summary>
			/// ユーザーデータ、ポリゴンインデックスをキー、ユーザーデータを値とする連想配列
			/// </summary>
			public Dictionary<int, object> UserData;

			/// <summary>
			/// このノードに接続されているエッジ一覧
			/// </summary>
			public List<Edge> Edges = new List<Edge>();

			/// <summary>
			/// コンストラクタ、インデックスと位置を指定して初期化する
			/// </summary>
			/// <param name="uniqueIndex">ユニークなインデックス</param>
			/// <param name="position">位置</param>
			public Node(uint uniqueIndex, vector position) {
				this.UniqueIndex = uniqueIndex;
				this.Position = position;
			}

			/// <summary>
			/// 指定のエッジへのリンクを追加する
			/// </summary>
			/// <param name="edge">エッジ</param>
			public void LinkEdge(Edge edge) {
				if (this.Edges.Contains(edge))
					return;
				this.Edges.Add(edge);
			}

			/// <summary>
			/// 指定のエッジへのリンクを削除する
			/// </summary>
			/// <param name="edge">エッジ</param>
			public void Remove(Edge edge) {
				if (!this.Edges.Contains(edge))
					return;
				this.Edges.Remove(edge);
			}

			/// <summary>
			/// 指定ポリゴン用のユーザーデータを設定する
			/// </summary>
			/// <param name="groupIndex">グループインデックス</param>
			/// <param name="userData">ユーザーデータ</param>
			public void SetUserData(int groupIndex, object userData) {
				if (this.UserData == null)
					this.UserData = new Dictionary<int, object>();
				this.UserData[groupIndex] = userData;
			}

			/// <summary>
			/// 指定ポリゴン用のユーザーデータを取得する
			/// </summary>
			/// <param name="groupIndex">グループインデックス</param>
			/// <returns>ユーザーデータ又は null</returns>
			public object GetUserData(int groupIndex) {
				if (this.UserData == null)
					return null;
				object obj;
				if (this.UserData.TryGetValue(groupIndex, out obj))
					return obj;
				else
					return null;
			}

			/// <summary>
			/// 指定されたポリゴンがリンクされているかどうか調べる
			/// </summary>
			/// <param name="groupIndex">グループインデックス</param>
			/// <returns>リンクされているなら true</returns>
			public bool IsPolygonLinked(int groupIndex) {
				var edges = this.Edges;
				for (int i = edges.Count - 1; i != -1; i--) {
					var edge = edges[i];
					if (edge.IsGroupLinked(groupIndex))
						return true;
				}
				return false;
			}

			/// <summary>
			/// 指定されたポリゴンがリンクされているかどうか調べる
			/// </summary>
			/// <param name="groupIndex">グループインデックス</param>
			/// <param name="excludeEdge">チェックから除外するエッジ</param>
			/// <returns>リンクされているなら true</returns>
			public bool IsPolygonLinked(int groupIndex, Edge excludeEdge) {
				var edges = this.Edges;
				for (int i = edges.Count - 1; i != -1; i--) {
					var edge = edges[i];
					if (edge != excludeEdge && edge.IsGroupLinked(groupIndex))
						return true;
				}
				return false;
			}

			public override string ToString() {
				return "node" + this.UniqueIndex;
			}

			//public static bool operator <(Node a, Node b) {
			//	if (a.Position.X < b.Position.X)
			//		return true;
			//	if (a.Position.X == b.Position.X)
			//		return a.Position.Y < b.Position.Y;
			//	else
			//		return false;
			//}

			//public static bool operator >(Node a, Node b) {
			//	if (a.Position.X > b.Position.X)
			//		return true;
			//	if (a.Position.X == b.Position.X)
			//		return a.Position.Y > b.Position.Y;
			//	else
			//		return false;
			//}
		}

		/// <summary>
		/// エッジ、ノード同士を繋ぎ位相情報を持たせる
		/// </summary>
		public class Edge {
			/// <summary>
			/// エッジの情報フラグ
			/// </summary>
			public EdgeFlags Flags;

			/// <summary>
			/// ユニークなエッジインデックス、 EdgeManager 内で同じ値があってはならない
			/// </summary>
			public uint UniqueIndex;

			/// <summary>
			/// 開始ノード
			/// </summary>
			public Node From;

			/// <summary>
			/// 終了ノード
			/// </summary>
			public Node To;

			/// <summary>
			/// エッジの長さ
			/// </summary>
			public element Length;

			/// <summary>
			/// 進行方向From→Toの右側のグループインデックス一覧
			/// </summary>
			public List<int> RightGroups = new List<int>();

			/// <summary>
			/// 進行方向From→Toの左側のグループインデックス一覧
			/// </summary>
			public List<int> LeftGroups = new List<int>();

			/// <summary>
			/// 進行方向From→Toの右側のグループ内ポリゴンインデックス一覧、[グループインデックス]
			/// </summary>
			public int[] RightPolygons;

			/// <summary>
			/// 進行方向From→Toの左側のグループ内ポリゴンインデックス一覧、[グループインデックス]
			/// </summary>
			public int[] LeftPolygons;

			/// <summary>
			/// 進行方向From→Toの右側のユーザーデータ、[グループインデックス]
			/// </summary>
			public object[] RightUserData;

			/// <summary>
			/// 進行方向From→Toの左側のユーザーデータ、[グループインデックス]
			/// </summary>
			public object[] LeftUserData;

			/// <summary>
			/// null でない場合にはエッジ上にノードを挿入する予定であることを示す
			/// </summary>
			public List<NodeInsertion> NodeInsertions;

			/// <summary>
			/// エッジの境界ボリュームの取得
			/// </summary>
			public volume Volume {
				get {
					return new volume(this.From.Position, this.To.Position, true);
				}
			}

			/// <summary>
			/// ノードの組み合わせからユニークなエッジIDを取得する
			/// </summary>
			public static ulong GetID(Node n1, Node n2) {
				var nid1 = n1.UniqueIndex;
				var nid2 = n2.UniqueIndex;
				return nid2 <= nid1 ? (ulong)nid1 << 32 | nid2 : (ulong)nid2 << 32 | nid1;
			}

			/// <summary>
			/// エッジの組み合わせからユニークな組み合わせIDを取得する
			/// ※エッジ同士の交差判定でテスト済みの組み合わせを再度テストしないために使う
			/// </summary>
			public static ulong GetCombID(Edge e1, Edge e2) {
				var nid1 = e1.UniqueIndex;
				var nid2 = e2.UniqueIndex;
				return nid2 <= nid1 ? (ulong)nid1 << 32 | nid2 : (ulong)nid2 << 32 | nid1;
			}

			/// <summary>
			/// コンストラクタ、インデックスとノードを指定して初期化する
			/// </summary>
			/// <param name="uniqueIndex">ユニークなインデックス</param>
			/// <param name="from">エッジの開始ノード</param>
			/// <param name="to">エッジの終了ノード</param>
			public Edge(uint uniqueIndex, Node from, Node to) {
				this.UniqueIndex = uniqueIndex;
				this.From = from;
				this.To = to;
				this.Length = (to.Position - from.Position).Length;
				from.LinkEdge(this);
				to.LinkEdge(this);
			}

			/// <summary>
			/// ノードから切断する
			/// </summary>
			public void Disconnect() {
				if (this.From != null) {
					this.From.Remove(this);
					this.From = null;
				}
				if (this.To != null) {
					this.To.Remove(this);
					this.To = null;
				}
			}

			/// <summary>
			/// ポリゴンへリンクする
			/// </summary>
			/// <param name="right">右側へリンクするなら true、左側なら false</param>
			/// <param name="groupIndex">グループインデックス</param>
			/// <param name="polygonIndex">グループ内のポリゴンインデックス</param>
			public void LinkPolygon(bool right, int groupIndex, int polygonIndex) {
				var g = right ? this.RightGroups : this.LeftGroups;
				if (g.Contains(groupIndex))
					return; // 既にリンク済みならスキップ　※同じ側に同グループのポリゴンはリンクされない仕様なので polygonIndex はチェックしなくて良い
				g.Add(groupIndex);

				var p = right ? this.RightPolygons : this.LeftPolygons;
				if (p == null) {
					p = new int[groupIndex + 1];
				} else if (p.Length <= groupIndex) {
					Array.Resize(ref p, groupIndex + 1);
				}
				p[groupIndex] = polygonIndex;
				if (right)
					this.RightPolygons = p;
				else
					this.LeftPolygons = p;
			}

			/// <summary>
			/// ポリゴンへリンクする
			/// </summary>
			/// <param name="from">ポリゴン内でのエッジの開始ノード</param>
			/// <param name="groupIndex">グループインデックス</param>
			/// <param name="polygonIndex">グループ内のポリゴンインデックス</param>
			public void LinkPolygon(Node from, int groupIndex, int polygonIndex) {
				LinkPolygon(from == this.From, groupIndex, polygonIndex);
			}

			/// <summary>
			/// 指定エッジの属性を方向を考慮しつつコピーする
			/// </summary>
			/// <param name="edge">エッジ</param>
			/// <param name="sameDir">指定エッジが同じ方向かどうか</param>
			public void CopyAttributes(Edge edge, bool sameDir) {
				List<int> rg, lg;
				int[] rp, lp;
				if (sameDir) {
					rg = edge.RightGroups;
					lg = edge.LeftGroups;
					rp = edge.RightPolygons;
					lp = edge.LeftPolygons;
				} else {
					lg = edge.RightGroups;
					rg = edge.LeftGroups;
					lp = edge.RightPolygons;
					rp = edge.LeftPolygons;
				}
				foreach (var g in rg) {
					this.LinkPolygon(true, g, rp[g]);
					var d = edge.GetUserData(sameDir, g);
					if (d != null)
						this.SetUserData(true, g, d);
				}
				foreach (var g in lg) {
					this.LinkPolygon(false, g, lp[g]);
					var d = edge.GetUserData(!sameDir, g);
					if (d != null)
						this.SetUserData(false, g, d);
				}
			}

			/// <summary>
			/// 指定されたグループがリンクされているかどうか調べる
			/// </summary>
			/// <param name="groupIndex">グループインデックス</param>
			/// <returns>リンクされているなら true</returns>
			public bool IsGroupLinked(int groupIndex) {
				return this.RightGroups.Contains(groupIndex) || this.LeftGroups.Contains(groupIndex);
			}

			/// <summary>
			/// From ノードから伸びる線分を取得する
			/// </summary>
			/// <param name="p">線分の開始位置</param>
			/// <param name="v">線分のベクトル</param>
			public void GetLine(out vector p, out vector v) {
				p = this.From.Position;
				v = this.To.Position - p;
			}

			/// <summary>
			/// 指定したノードから伸びる線分を取得する
			/// </summary>
			/// <param name="from">線分の開始ノード</param>
			/// <param name="p">線分の開始位置</param>
			/// <param name="v">線分のベクトル</param>
			public void GetLine(Node from, out vector p, out vector v) {
				if (from == this.From) {
					p = this.From.Position;
					v = this.To.Position - p;
				} else if (from == this.To) {
					p = this.To.Position;
					v = this.From.Position - p;
				} else {
					throw new ApplicationException();
				}
			}

			/// <summary>
			/// 指定グループ用のユーザーデータを設定する
			/// </summary>
			/// <param name="right">true なら右側に false なら左側に設定する</param>
			/// <param name="groupIndex">グループインデックス</param>
			/// <param name="userData">ユーザーデータ</param>
			public void SetUserData(bool right, int groupIndex, object userData) {
				var d = right ? this.RightUserData : this.LeftUserData;
				if (d == null) {
					d = new object[groupIndex + 1];
				} else if (d.Length <= groupIndex) {
					Array.Resize(ref d, groupIndex + 1);
				}
				d[groupIndex] = userData;
				if (right) {
					this.RightUserData = d;
				} else {
					this.LeftUserData = d;
				}
			}

			/// <summary>
			/// 指定グループ用のユーザーデータを取得する
			/// </summary>
			/// <param name="right">true なら右側に false なら左側から取得する</param>
			/// <param name="groupIndex">グループインデックス</param>
			/// <returns>ユーザーデータ又は null</returns>
			public object GetUserData(bool right, int groupIndex) {
				var d = right ? this.RightUserData : this.LeftUserData;
				if (d == null || d.Length <= groupIndex)
					return null;
				return d[groupIndex];
			}

			/// <summary>
			/// 位相構造上指定エッジと交差する可能性があるかどうか調べる
			/// </summary>
			/// <param name="edge">エッジ</param>
			/// <returns>交差し得るなら true</returns>
			public bool IsCrossable(Edge edge) {
				// 同じエッジなら交差するはずがない
				if (this == edge)
					return false;

				// ポリゴンは自己交差しない前提なので
				// リンクしているポリゴンが両方とも同じなら交差しない
				var set1 = new List<int>();
				var set2 = new List<int>();

				foreach (var p in this.RightGroups)
					set1.Add(p);
				foreach (var p in this.LeftGroups)
					set1.Add(p);
				foreach (var p in edge.RightGroups)
					set2.Add(p);
				foreach (var p in edge.LeftGroups)
					set2.Add(p);

				foreach (var p in set1) {
					if (!set2.Contains(p))
						return true;
				}
				foreach (var p in set2) {
					if (!set1.Contains(p))
						return true;
				}

				return false;
			}

			public void SetNodeInsertion(element t, Node node) {
				if (this.NodeInsertions == null)
					this.NodeInsertions = new List<NodeInsertion>();
				this.NodeInsertions.Add(new NodeInsertion(t, node));
			}

			public override string ToString() {
				return "edge" + this.UniqueIndex + "(" + this.From.UniqueIndex + ", " + this.To.UniqueIndex + ")";
			}
		}

		/// <summary>
		/// バリデーション結果
		/// </summary>
		public struct ValidationResult {
			/// <summary>
			/// 有効かどうか
			/// </summary>
			public bool IsValid;

			/// <summary>
			/// エラーがあるならエラー内容メッセージが入る
			/// </summary>
			public string Message;

			public ValidationResult(bool isValid, string message) {
				this.IsValid = isValid;
				this.Message = message;
			}
		}

		public struct PointWithId {
			public int Id;
			public vector P;

			public PointWithId(int id, vector p) {
				this.Id = id;
				this.P = p;
			}
		}

		public struct LineWithId {
			public Polygon Polygon;
			public int Id;
			public vector P1;
			public vector P2;

			public LineWithId(Polygon polygon, int id, vector p1, vector p2) {
				this.Polygon = polygon;
				this.Id = id;
				this.P1 = p1;
				this.P2 = p2;
			}
		}

		/// <summary>
		/// 座標の範囲で頂点間のラインを検索するヘルパクラス
		/// </summary>
		public class LineFinder {
			DynamicAABB2fTree<LineWithId> _Tree = new DynamicAABB2fTree<LineWithId>();

			public void Add(Polygon polygon, int startId, List<Vertex> vertices, element epsilon) {
				var tree = _Tree;
				var nvts = vertices.Count;
				var v1 = vertices[0].Position;

				startId--;

				for (int i = 1, n = nvts + 1; i < n; i++) {
					var v2 = vertices[i % nvts].Position;
					var id = startId + i;
					var volume = new volume(v1, v2, true).Expand(epsilon);

					tree.Add(volume, new LineWithId(polygon, id, v1, v2));

					v1 = v2;
				}
			}

			public void Optimize() {
				_Tree.OptimizeTopDown();
			}

			/// <summary>
			/// 指定範囲内の頂点を検索しインデックス番号配列を取得する
			/// </summary>
			/// <param name="volume">検索範囲を表す境界ボリューム</param>
			/// <returns>範囲内の頂点インデックス番号配列</returns>
			public List<LineWithId> Query(volume volume) {
				return new List<LineWithId>(_Tree.Query(volume));
			}

			/// <summary>
			/// 指定線分と交差する線分があるか調べる
			/// </summary>
			/// <param name="p1">線分の点１</param>
			/// <param name="p2">線分の点２</param>
			/// <param name="polygon">除外するIDのオブジェクト</param>
			/// <param name="id1">除外する線分ID１</param>
			/// <param name="id2">除外する線分ID２</param>
			/// <param name="id3">除外する線分ID３</param>
			/// <returns>接触しているならtrue</returns>
			public bool TestIntersect(vector p1, vector p2, object polygon, int id1, int id2, int id3) {
				var qs = Query(new volume(p1, p2, true));
				for (int i = qs.Count - 1; i != -1; i--) {
					var q = qs[i];
					if ((q.Polygon != polygon || q.Id != id1 && q.Id != id2 && q.Id != id3) && LineIntersect(p1, p2, q.P1, q.P2)) {
						return true;
					}
				}
				return false;
			}

			/// <summary>
			/// 指定線分と同じ座標の線分があるか調べる
			/// </summary>
			/// <param name="p1">線分の点１</param>
			/// <param name="p2">線分の点２</param>
			/// <param name="polygon">除外するIDのオブジェクト</param>
			/// <param name="id">除外する線分ID</param>
			/// <param name="epsilon">同一座標判定用最小距離値の二乗、この距離以下の距離は同一座標とみなす</param>
			/// <returns>接触しているならtrue</returns>
			public bool TestShare(vector p1, vector p2, object polygon, int id, element epsilon2) {
				if (p2 < p1) {
					var t = p1;
					p1 = p2;
					p2 = t;
				}

				var qs = Query(new volume(p1, p2, true));
				for (int i = qs.Count - 1; i != -1; i--) {
					var q = qs[i];
					if (q.Polygon != polygon || q.Id == id)
						continue;

					if (q.P2 < q.P1) {
						var t = q.P1;
						q.P1 = q.P2;
						q.P2 = t;
					}

					if ((q.P1 - p1).LengthSquare <= epsilon2 && (q.P2 - p2).LengthSquare <= epsilon2)
						return true;
				}
				return false;
			}
		}

		/// <summary>
		/// ノードとエッジで構成されるポリゴン
		/// </summary>
		public class TopologicalPolygon {
			/// <summary>
			/// エッジ配列
			/// </summary>
			public List<EdgeAndSide> Edges;

			/// <summary>
			/// 穴配列
			/// </summary>
			public List<TopologicalPolygon> Holes;

			/// <summary>
			/// ポリゴンの境界ボリューム
			/// </summary>
			public volume Volume;

			/// <summary>
			/// ポリゴンの面積
			/// </summary>
			public element Area;

			/// <summary>
			/// 時計回りかどうか
			/// </summary>
			public bool CW;

			/// <summary>
			/// 指定座標を包含しているか調べる
			/// </summary>
			/// <param name="c">座標</param>
			/// <returns>包含しているなら true</returns>
			public bool Contains(vector c) {
				if (PointTouchPolygon2(c, this.Edges, true) == 0)
					return false;
				var holes = this.Holes;
				if (holes != null) {
					for (int i = holes.Count - 1; i != -1; i--) {
						var hole = holes[i];
						if (PointTouchPolygon2(c, hole.Edges, true) != 0)
							return false;
					}
				}
				return true;
			}

			/// <summary>
			/// 指定ポリゴンのエッジを完全に包含しているか調べる
			/// </summary>
			/// <param name="thisGroupIndex">自分のグループインデックス</param>
			/// <param name="polygon">包含チェック対象ポリゴン</param>
			/// <returns>包含しているなら true</returns>
			public bool Contains(int thisGroupIndex, TopologicalPolygon polygon) {
				// 面積が小さいなら包含はあり得ない
				if (this.Area < polygon.Area)
					return false;

				// 境界ボリュームが完全に包含されていないなら包含はあり得ない
				if (!this.Volume.Contains(polygon.Volume))
					return false;

				// エッジを共有しているなら PolygonizeAll() で勝手に調整されるので包含されていないことにする
				var edges = polygon.Edges;
				var nedges = edges.Count;
				for (int i = nedges - 1; i != -1; i--) {
					if (edges[i].Edge.IsGroupLinked(thisGroupIndex))
						return false;
				}

				// this と polygon が共用しているノードを1とし、それ以外を0とする
				// 0→1へ変化した際の0と1→0へ変化した際の0がポリゴンに包含されているか調べる
				// 0↔1の変化が無い場合には適当に選んだノードの包含を調べる
				// 又、1のノードに貫通フラグがセットされていたら貫通してるということなので包含はあり得ない　※これは高速化のため
				var intersects = false;
				var edge1 = edges[0];
				var node1 = edge1.From;
				var type1 = node1.IsPolygonLinked(thisGroupIndex, edge1.Edge);
				for (int i = nedges - 1; i != -1; i--) {
					if (type1 && (node1.Flags & NodeFlags.InsideOutside) != 0)
						return false; // TODO: 貫通フラグ見てるけど計算ポリゴン数２つだからこそできてる

					var edge2 = edges[i];
					var node2 = edge2.From;
					var type2 = node2.IsPolygonLinked(thisGroupIndex, edge2.Edge);
					if (type1 != type2) {
						if (!Contains(type1 ? node2.Position : node1.Position))
							return false;
						intersects = true;
					}
					node1 = node2;
					type1 = type2;
				}
				if (!intersects) {
					if (!Contains(node1.Position))
						return false;
				}

				// 上記を全てパスしたら包含されていることになる
				return true;
			}

			/// <summary>
			/// エッジが指定ポリゴンに包含されているなら指定ポリゴンインデックスをリンクする
			/// </summary>
			/// <param name="polygon">包含元グループ</param>
			/// <param name="groupIndex">包含元グループインデックス</param>
			/// <param name="polygonIndex">包含元ポリゴンインデックス</param>
			public void LinkPolygonIfContained(TopologicalPolygon polygon, int groupIndex, int polygonIndex) {
				if (polygon.Contains(groupIndex, this)) {
					var edges = this.Edges;
					for (int i = edges.Count - 1; i != -1; i--) {
						var edge = edges[i];
						edge.Edge.LinkPolygon(true, groupIndex, polygonIndex);
						edge.Edge.LinkPolygon(false, groupIndex, polygonIndex);
					}
				}
				var holes = this.Holes;
				if (holes != null) {
					for (int holeIndex = holes.Count - 1; holeIndex != -1; holeIndex--) {
						holes[holeIndex].LinkPolygonIfContained(polygon, groupIndex, polygonIndex);
					}
				}
			}
		}

		/// <summary>
		/// ノード管理クラス
		/// </summary>
		class NodeManager {
			uint _UniqueIndex;
			HashSet<Node> _Nodes = new HashSet<Node>();
			DynamicAABB2fTree<Node> _Tree = new DynamicAABB2fTree<Node>();
			element _Epsilon;

			/// <summary>
			/// 全ノード一覧の取得
			/// </summary>
			public ICollection<Node> Items {
				get {
					return _Nodes;
				}
			}

			/// <summary>
			/// コンストラクタ
			/// </summary>
			/// <param name="epsilon">ノードの半径</param>
			public NodeManager(element epsilon) {
				_Epsilon = epsilon;
			}

			/// <summary>
			/// ノードを新規作成する、指定ノード位置に接触するノードが既に存在する場合そのノードを返す
			/// </summary>
			/// <param name="position">ノード位置</param>
			/// <returns>ノード</returns>
			public Node New(vector position) {
				// ノードの境界ボリューム計算
				var volume = new AABB2f(position).Expand(_Epsilon);

				// 境界ボリューム同士が接触するノード一覧取得
				// ノード一覧内で最も距離が近いものを探す
				Node node = null;
				var mindist2 = _Epsilon * _Epsilon;
				foreach(var nd in _Tree.Query(volume)) {
					var dist2 = (position - nd.Position).LengthSquare;
					if (dist2 < mindist2) {
						node = nd;
						mindist2 = dist2;
					}
				}

				// 接触しているノードが無かった場合のみ新規作成
				if (node == null) {
					node = new Node(++_UniqueIndex, position);
					_Nodes.Add(node);
					_Tree.Add(volume, node);
				}

				return node;
			}

			/// <summary>
			/// 指定されたノードを取り除く
			/// </summary>
			/// <param name="node">ノード</param>
			public void Remove(Node node) {
				if (!_Nodes.Contains(node))
					return;
				_Nodes.Remove(node);

				var volume = new AABB2f(node.Position);
				var leaf = (from r in _Tree.QueryLeaves(volume) where r.Value == node select r).FirstOrDefault();
				if (leaf != null)
					_Tree.Remove(leaf);
			}

			/// <summary>
			/// 指定された境界ボリュームに接触するノード一覧取得
			/// </summary>
			/// <param name="volume">境界ボリューム</param>
			/// <returns>ノード一覧</returns>
			public IEnumerable<Node> Query(volume volume) {
				return _Tree.Query(volume);
			}

			/// <summary>
			/// 座標による検索用ツリーを最適化する
			/// </summary>
			public void Optimize() {
				_Tree.OptimizeTopDown();
			}
		}

		/// <summary>
		/// エッジ管理クラス
		/// </summary>
		class EdgeManager {
			uint _UniqueIndex;
			Dictionary<ulong, Edge> _Edges = new Dictionary<ulong, Edge>();
			DynamicAABB2fTree<Edge> _Tree = new DynamicAABB2fTree<Edge>();
			element _Epsilon;

			/// <summary>
			/// 全エッジ一覧の取得
			/// </summary>
			public ICollection<Edge> Items {
				get {
					return _Edges.Values;
				}
			}

			/// <summary>
			/// コンストラクタ
			/// </summary>
			/// <param name="epsilon">ノード半径</param>
			public EdgeManager(element epsilon) {
				_Epsilon = epsilon;
			}

			/// <summary>
			/// エッジを新規作成する、指定ノード２つに接続されているエッジが既に存在したらそのエッジを返す
			/// </summary>
			/// <param name="from"></param>
			/// <param name="to"></param>
			/// <returns>エッジ</returns>
			public Edge New(Node from, Node to) {
				var id = Edge.GetID(from, to);
				Edge edge;
				if (_Edges.TryGetValue(id, out edge)) {
					return edge;
				} else {
					edge = new Edge(++_UniqueIndex, from, to);
					_Edges[id] = edge;
					_Tree.Add(edge.Volume.Expand(_Epsilon), edge);
					return edge;
				}
			}

			/// <summary>
			/// 指定されたエッジを取り除く
			/// </summary>
			/// <param name="edge">エッジ</param>
			public void Remove(Edge edge) {
				var id = Edge.GetID(edge.From, edge.To);
				if (!_Edges.ContainsKey(id))
					return;
				_Edges.Remove(id);

				var volume = edge.Volume.Expand(_Epsilon);
				var leaf = (from r in _Tree.QueryLeaves(volume) where r.Value == edge select r).FirstOrDefault();
				if (leaf != null)
					_Tree.Remove(leaf);

				edge.Disconnect();
			}

			/// <summary>
			/// 指定された境界ボリュームに接触するエッジ一覧取得
			/// </summary>
			/// <param name="volume">境界ボリューム</param>
			/// <returns>エッジ一覧</returns>
			public IEnumerable<Edge> Query(volume volume) {
				return _Tree.Query(volume);
			}

			/// <summary>
			/// 座標による検索用ツリーを最適化する
			/// </summary>
			public void Optimize() {
				_Tree.OptimizeTopDown();
			}
		}

		/// <summary>
		/// エッジと方向
		/// </summary>
		public struct EdgeAndSide {
			/// <summary>
			/// エッジ
			/// </summary>
			public Edge Edge;

			/// <summary>
			/// 方向、true なら順方向、false なら逆方向
			/// </summary>
			public bool TraceRight;

			public EdgeAndSide(Edge edge, bool right) {
				this.Edge = edge;
				this.TraceRight = right;
			}

			public Node From {
				get {
					return this.TraceRight ? this.Edge.From : this.Edge.To;
				}
			}

			public override string ToString() {
				var from = this.TraceRight ? this.Edge.From : this.Edge.To;
				var to = this.TraceRight ? this.Edge.To : this.Edge.From;
				return string.Format("{{ {0} => {1} {2} }}", from.Position, to.Position, this.TraceRight ? "Right" : "Left");
			}
		}

		/// <summary>
		/// 配列のインデックス番号範囲を示す
		/// </summary>
		public struct IndexRange {
			/// <summary>
			/// インデックス開始値
			/// </summary>
			public int Start;

			/// <summary>
			/// 要素数
			/// </summary>
			public int Count;

			/// <summary>
			/// コンストラクタ、インデックス開始値と要素数を指定して初期化する
			/// </summary>
			/// <param name="start">インデックス開始値</param>
			/// <param name="count">要素数</param>
			public IndexRange(int start, int count) {
				this.Start = start;
				this.Count = count;
			}
		}

		/// <summary>
		/// 結果のループデータ
		/// </summary>
		public class Loop {
			/// <summary>
			/// 面積
			/// </summary>
			public element Area;

			/// <summary>
			/// 時計回りかどうか
			/// </summary>
			public bool CW;

			/// <summary>
			/// ループを構成するエッジ配列
			/// </summary>
			public List<EdgeAndSide> Edges;

			/// <summary>
			/// 境界ボリューム
			/// </summary>
			public volume Volume;

			public Loop(TopologicalPolygon tpol) {
				this.Area = tpol.Area;
				this.CW = tpol.CW;
				this.Edges = tpol.Edges;
				this.Volume = tpol.Volume;
			}

			public Loop(element area, List<EdgeAndSide> edges) {
				this.Area = Math.Abs(area);
				this.CW = area <= 0;
				this.Edges = edges;

				var volume = new volume(edges[0].From.Position);
				for (int i = 0, n = edges.Count; i < n; i++) {
					volume.MergeSelf(edges[i].From.Position);
				}

				this.Volume = volume;
			}
		}

		/// <summary>
		/// 交点ノードに対する処理を行うデリゲート
		/// </summary>
		/// <param name="edge1">交差エッジ１</param>
		/// <param name="t1">エッジ１の交点パラメータ</param>
		/// <param name="edge2">交差エッジ２</param>
		/// <param name="t2">エッジ２の交点パラメータ</param>
		/// <param name="newNode">交点のノード</param>
		public delegate void IntersectionNodeProc(Edge edge1, element t1, Edge edge2, element t2, Node newNode);
		#endregion

		#region フィールド
		NodeManager _NodeMgr;
		EdgeManager _EdgeMgr;
		List<List<Polygon>> _Groups = new List<List<Polygon>>();
		List<List<TopologicalPolygon>> _TopoGroups = new List<List<TopologicalPolygon>>();
		element _Epsilon;
		IntersectionNodeProc _IntersectionNodeGenerator;
#if POLYGONBOOLEAN_DEBUG
		static bool _Logging;
#endif
		#endregion

		#region プロパティ
		/// <summary>
		/// ノード一覧の取得
		/// </summary>
		public ICollection<Node> Nodes {
			get {
				return _NodeMgr.Items;
			}
		}

		/// <summary>
		/// エッジ一覧の取得
		/// </summary>
		public ICollection<Edge> Edges {
			get {
				return _EdgeMgr.Items;
			}
		}

		/// <summary>
		/// エッジ交点にノード挿入する際の交点ノード処理デリゲート
		/// </summary>
		public IntersectionNodeProc IntersectionNodeGenerator {
			get {
				return _IntersectionNodeGenerator;
			}

			set {
				_IntersectionNodeGenerator = value;
			}
		}

		/// <summary>
		/// 処理対象のポリゴングループ一覧
		/// </summary>
		public List<List<Polygon>> Groups {
			get {
				return _Groups;
			}
		}
		#endregion

		#region 公開メソッド
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="epsilon">頂点同士、エッジと頂点の距離の最小値、これより距離が近い場合には距離０として扱い同じノードになる</param>
		public PolBoolF(element epsilon) {
			_NodeMgr = new NodeManager(epsilon);
			_EdgeMgr = new EdgeManager(epsilon);
			_Epsilon = epsilon;
		}

		/// <summary>
		/// ポリゴンを登録する
		/// </summary>
		/// <param name="polygons">ポリゴン</param>
		/// <returns>追加されたポリゴンのインデックス</returns>
		public int AddPolygon(IEnumerable<Polygon> polygons) {
			var result = _Groups.Count;
			_Groups.Add(new List<Polygon>(polygons));
			return result;
		}

		/// <summary>
		/// ポリゴンをノードとエッジに分解する、ポリゴン同士の交点にはノードが追加される
		/// </summary>
		/// <param name="validation">入力ポリゴンからトポロジー作成可能か調べるならtrue</param>
		/// <returns>全てのポリゴンを構成するエッジと方向の一覧</returns>
		public void CreateTopology(bool validation) {
			// ポリゴンが使用可能か調べる
			if (validation) {
				var epsilon = _Epsilon;
				for (int groupIndex = 0, ngroups = _Groups.Count; groupIndex < ngroups; groupIndex++) {
					var group = _Groups[groupIndex];
					var npolygons = group.Count;
					var lfinder = new LineFinder();

					for (int polygonIndex = 0; polygonIndex < npolygons; polygonIndex++) {
						var pol = group[polygonIndex];
						var result = pol.PrepareValidation(lfinder, epsilon);
						if (!result.IsValid) {
							throw new ApplicationException("ポリゴン" + (groupIndex + 1) + ": " + result.Message);
						}
					}

					lfinder.Optimize();

					for (int polygonIndex = 0; polygonIndex < npolygons; polygonIndex++) {
						var pol = group[polygonIndex];
						var result = pol.Validation(lfinder, epsilon);
						if (!result.IsValid) {
							throw new ApplicationException("ポリゴン" + (groupIndex + 1) + ": " + result.Message);
						}
					}
				}
			}

			// ポリゴンからノードとエッジを作成する
			MakeNodesAndEdges();

			// 交点にノードを挿入する
			MakeIntersectionNodes();

			// ヒゲを取り除く
			RemoveBeard();

			// _TopologicalPolygons を再構成する
			RebuildTpols();

			// tpol が他の tpol に包含されているかチェックし包含されているならエッジに他のtpolをリンクする
			TpolsInclusionCheck();

			// ポリゴンを構成する
			PolygonizeAll();
		}

		/// <summary>
		/// エッジを共有しているグループインデックスを取得する
		/// </summary>
		/// <returns>グループインデックス配列</returns>
		public List<int> GetEdgeSharedGroups() {
			var groups = new List<int>();
			var linkedGroups = new List<int>(_Groups.Count);

			foreach (var edge in this.Edges) {
				var rg = edge.RightGroups;
				var lg = edge.LeftGroups;

				linkedGroups.Clear();

				for (int i = rg.Count - 1; i != -1; i--) {
					var g = rg[i];
					if (!linkedGroups.Contains(g)) {
						linkedGroups.Add(g);
					}
				}

				for (int i = lg.Count - 1; i != -1; i--) {
					var g = lg[i];
					if (!linkedGroups.Contains(g)) {
						linkedGroups.Add(g);
					}
				}

				if (2 <= linkedGroups.Count) {
					for (int i = linkedGroups.Count - 1; i != -1; i--) {
						var g = linkedGroups[i];
						if (!groups.Contains(g)) {
							groups.Add(g);
						}
					}
				}
			}

			return groups;
		}

		/// <summary>
		/// ポリゴンを構成するエッジと方向の一覧からノード一覧を生成する
		/// </summary>
		/// <param name="edges">エッジと方向の一覧</param>
		/// <returns>ノード一覧</returns>
		public static List<Node> NodesFromEdges(IEnumerable<EdgeAndSide> edges) {
			return new List<Node>(from r in edges select r.From);
		}

		/// <summary>
		/// 指定フィルタにマッチする弧線リストを取得する
		/// </summary>
		/// <param name="edges">方向付きのエッジリスト</param>
		/// <param name="matcher">マッチ判定デリゲート</param>
		/// <returns>弧線リスト</returns>
		public static List<IndexRange> MatchSegments(List<EdgeAndSide> edges, Func<Edge, bool, bool> matcher) {
			var list = new List<IndexRange>();
			var n = edges.Count;

			// まずマッチしないエッジを探す
			int start = -1;
			for (int i = 0; i < n; i++) {
				var e = edges[i];
				if (!matcher(e.Edge, e.TraceRight)) {
					start = i;
					break;
				}
			}
			if (start == -1)
				return list; // 見つからなかったら終了

			// マッチしないエッジとマッチするエッジの境目を探していく
			var lastMatched = false;
			var indexRange = new IndexRange(0, 0);
			for (int i = 1; i <= n; i++) {
				var index = (start + i) % n;
				var e = edges[index];
				var match = matcher(e.Edge, e.TraceRight);
				if (match) {
					if (!lastMatched) {
						indexRange.Start = index;
						indexRange.Count = 1;
					} else {
						indexRange.Count++;
					}
				} else {
					if (lastMatched) {
						list.Add(indexRange);
						indexRange = new IndexRange(0, 0);
					}
				}
				lastMatched = match;
			}

			return list;
		}

		/// <summary>
		/// 指定されたエッジフィルタをパスしたエッジからポリゴンを作成する
		/// </summary>
		/// <param name="edgeFilter">フィルタ、エッジと右方向かどうかを受け取り無視するなら true を返す</param>
		/// <returns>結果のポリゴンを構成するエッジと方向の一覧</returns>
		/// <remarks>事前に CreateTopology() を呼び出しておく必要がある。</remarks>
		public List<List<Loop>> Filtering(Func<Edge, bool, bool> edgeFilter) {
			return Distinguish(GetPolygons(edgeFilter, EdgeFlags.RightRemoved, EdgeFlags.LeftRemoved));
		}

		/// <summary>
		/// AddPolygon() で登録されたポリゴン同士のORポリゴンを作成する
		/// </summary>
		/// <returns>結果のポリゴンを構成するエッジと方向の一覧</returns>
		/// <remarks>事前に CreateTopology() を呼び出しておく必要がある。</remarks>
		public List<List<Loop>> Or() {
#if POLYGONBOOLEAN_DEBUG
			_Logging = true;
			System.Diagnostics.POLYGONBOOLEAN_DEBUG.WriteLine("======== Or ========");
#endif
			// エッジの両側にポリゴンが存在するなら無視するフィルタ
			var edgeFilter = new Func<Edge, bool, bool>(
				(Edge e, bool right) => {
					return e.LeftGroups.Count != 0 && e.RightGroups.Count != 0;
				}
			);
#if POLYGONBOOLEAN_DEBUG
			try {
#endif
				return Filtering(edgeFilter);
#if POLYGONBOOLEAN_DEBUG
			} finally {
				_Logging = false;
			}
#endif
		}

		/// <summary>
		/// AddPolygon() で登録されたポリゴン同士のXORポリゴンを作成する
		/// </summary>
		/// <returns>結果のポリゴンを構成するエッジと方向の一覧</returns>
		/// <remarks>事前に CreateTopology() を呼び出しておく必要がある。</remarks>
		public List<List<Loop>> Xor() {
#if POLYGONBOOLEAN_DEBUG
			_Logging = true;
			System.Diagnostics.POLYGONBOOLEAN_DEBUG.WriteLine("======== Xor ========");
#endif
			// エッジの両側のポリゴン数が同じか、指定方向に偶数ポリゴンが存在するなら無視するフィルタ
			var edgeFilter = new Func<Edge, bool, bool>(
				(Edge e, bool right) => {
					if (e.LeftGroups.Count == e.RightGroups.Count)
						return true;
					return (right ? e.RightGroups : e.LeftGroups).Count % 2 == 0;
				}
			);

#if POLYGONBOOLEAN_DEBUG
			try {
#endif
				return Filtering(edgeFilter);
#if POLYGONBOOLEAN_DEBUG
			} finally {
				_Logging = false;
			}
#endif
		}

		/// <summary>
		/// AddPolygon() で登録されたポリゴン同士のANDポリゴンを作成する
		/// </summary>
		/// <returns>結果のポリゴンを構成するエッジと方向の一覧</returns>
		/// <remarks>事前に CreateTopology() を呼び出しておく必要がある。</remarks>
		public List<List<Loop>> And() {
			var allpols = new int[_TopoGroups.Count];
			for (int i = 0, n = allpols.Length; i < n; i++)
				allpols[i] = i;

			// エッジの指定方向に登録ポリゴンの内一つでも存在しないなら無視するフィルタ
			var edgeFilter = new Func<Edge, bool, bool>(
				(Edge e, bool right) => {
					var pols = right ? e.RightGroups : e.LeftGroups;
					foreach (var p in allpols) {
						if (!pols.Contains(p))
							return true;
					}
					return false;
				}
			);

			return Filtering(edgeFilter);
		}

		/// <summary>
		/// 指定されたインデックスのポリゴンを減算したポリゴンを作成する
		/// </summary>
		/// <param name="groupIndex">減算するグループのインデックス</param>
		/// <returns>結果のポリゴンを構成するエッジと方向の一覧</returns>
		/// <remarks>事前に CreateTopology() を呼び出しておく必要がある。</remarks>
		public List<List<Loop>> Sub(int groupIndex) {
			// エッジの指定方向に減算ポリゴンが存在するなら無視するフィルタ
			var edgeFilter = new Func<Edge, bool, bool>(
				(Edge e, bool right) => {
					return (right ? e.RightGroups : e.LeftGroups).Contains(groupIndex);
				}
			);
			return Filtering(edgeFilter);
		}

		/// <summary>
		/// 指定されたインデックスのポリゴンのみを抽出したポリゴンを作成する
		/// </summary>
		/// <param name="groupIndex">抽出するグループのインデックス</param>
		/// <returns>結果のポリゴンを構成するエッジと方向の一覧</returns>
		/// <remarks>事前に CreateTopology() を呼び出しておく必要がある。</remarks>
		public List<List<Loop>> Extract(int groupIndex) {
			var list = new List<List<Loop>>();
			var tpols = _TopoGroups[groupIndex];

			for(int i = 0, m = tpols.Count; i < m; i++) {
				var tpol = tpols[i];
				var polygons = new List<Loop>();

				polygons.Add(new Loop(tpol));
				var holes = tpol.Holes;
				if (holes != null) {
					for (int hodeIndex = 0, n = holes.Count; hodeIndex < n; hodeIndex++) {
						polygons.Add(new Loop(holes[hodeIndex]));
					}
				}

				list.Add(polygons);
			}

			return list;
		}
		#endregion

		#region 非公開メソッド
		/// <summary>
		/// 指定されたフィルタでポリゴンを取得する
		/// </summary>
		/// <param name="edgeFilter">フィルタ</param>
		/// <param name="onlyRight">エッジ右側のみから取得するかどうか</param>
		/// <returns>エッジによるポリゴンの配列</returns>
		private List<List<EdgeAndSide>> GetPolygons(Func<Edge, bool, bool> edgeFilter, EdgeFlags rightFlag, EdgeFlags leftFlag, bool onlyRight = false) {
			var polygons = new List<List<EdgeAndSide>>();

			// 予め無視することがわかっているエッジを処理
			var flagsnot = ~(rightFlag | leftFlag);
			foreach (var edge in this.Edges) {
				edge.Flags &= flagsnot;
				if (edgeFilter(edge, true))
					edge.Flags |= rightFlag;
				if (edgeFilter(edge, false))
					edge.Flags |= leftFlag;
			}

			// 右側にまだポリゴンが存在するエッジのみ処理する
			foreach (var edge in this.Edges) {
				if ((edge.Flags & rightFlag) == 0) {
					// ポリゴンを構成するエッジと方向一覧を取得
					bool isNull;
					var edges = TracePolygon(edge, true, false, rightFlag, leftFlag, out isNull);
					// 結果のポリゴン一覧に追加
					if (!isNull)
						polygons.Add(edges);
				}
			}

			// 左側にまだポリゴンが存在するエッジのみ処理する
			if (!onlyRight) {
				foreach (var edge in this.Edges) {
					if ((edge.Flags & leftFlag) == 0) {
						// ポリゴンを構成するエッジと方向一覧を取得
						bool isNull;
						var edges = TracePolygon(edge, false, false, rightFlag, leftFlag, out isNull);
						// 結果のポリゴン一覧に追加
						if (!isNull)
							polygons.Add(edges);
					}
				}
			}

			return polygons;
		}

		/// <summary>
		/// 全エッジのうちポリゴンを構成できるところを全て構成する
		/// </summary>
		/// <returns>全てのポリゴンを構成するエッジと方向の一覧</returns>
		private void PolygonizeAll() {
			// 未処理のエッジのみ処理する
			foreach (var edge in this.Edges) {
				if ((edge.Flags & EdgeFlags.RightPolygonized) == 0)
					Polygonize(edge, true);
			}
			foreach (var edge in this.Edges) {
				if ((edge.Flags & EdgeFlags.LeftPolygonized) == 0)
					Polygonize(edge, false);
			}
		}

		/// <summary>
		/// 指定エッジの指定側を辿りポリゴン化する
		/// </summary>
		/// <param name="edge">エッジ</param>
		/// <param name="traceRight">右側を辿るなら true 、左側を辿るなら false</param>
		private static void Polygonize(Edge edge, bool traceRight) {
			bool isNull;
			var list = TracePolygon(edge, traceRight, false, EdgeFlags.RightPolygonized, EdgeFlags.LeftPolygonized, out isNull);
			if (isNull)
				return;

			// エッジの指定方向にあるグループ＆ポリゴン一覧を作成
			var groupPolygons = new HashSet<ulong>();
			for (int i = list.Count - 1; i != -1; i--) {
				var eas = list[i];
				List<int> groups;
				int[] polygons;
				if (eas.TraceRight) {
					groups = eas.Edge.RightGroups;
					polygons = eas.Edge.RightPolygons;
				} else {
					groups = eas.Edge.LeftGroups;
					polygons = eas.Edge.LeftPolygons;
				}
				for (int j = groups.Count - 1; j != -1; j--) {
					var g = groups[j];
					var p = polygons[g];
					groupPolygons.Add((ulong)g << 32 | (ulong)(uint)p);
				}
			}

			// ポリゴンを構成するエッジのポリゴン情報を統一する
			for (int i = list.Count - 1; i != -1; i--) {
				var eas = list[i];
				foreach (var gp in groupPolygons) {
					var g = (int)(gp >> 32);
					var p = (int)(gp & 0xffffffff);
					eas.Edge.LinkPolygon(eas.TraceRight, g, p);
				}
			}
		}

		/// <summary>
		/// 指定されたエッジの指定側を辿りポリゴンを構成するエッジ一覧を取得する
		/// </summary>
		/// <param name="edge">開始エッジ、このポリゴンの右側を辿る</param>
		/// <param name="traceRight">エッジの右側を辿るなら true 、左側を辿るなら false</param>
		/// <param name="traceCCW">true なら最も反時計回り側のエッジを辿る、false なら最も時計回り側のエッジを辿る</param>
		/// <param name="isNull">指定された側にポリゴン形成できないなら true が返る</param>
		/// <returns>ポリゴンを構成するエッジと方向の一覧</returns>
		private static List<EdgeAndSide> TracePolygon(Edge edge, bool traceRight, bool traceCCW, EdgeFlags rightFlag, EdgeFlags leftFlag, out bool isNull) {
			var list = new List<EdgeAndSide>();
			var startEdge = edge;
			var startNode = traceRight ? edge.From : edge.To; // ポリゴンの開始ノード
			var nextNode = traceRight ? edge.To : edge.From;
			var curNode = startNode;
			var curIsRight = traceRight; // エッジの右側を辿るなら true、左側なら false
			var isNullInternal = true;

#if POLYGONBOOLEAN_DEBUG
			if(_Logging) {
				System.Diagnostics.POLYGONBOOLEAN_DEBUG.WriteLine("==== " + (traceRight ? "Right" : "Left") + " ====");
			}
#endif

			while (true) {
				// ポリゴンを構成するエッジとして方向と共に登録
				list.Add(new EdgeAndSide(edge, curIsRight));
				edge.Flags |= curIsRight ? rightFlag : leftFlag;

#if POLYGONBOOLEAN_DEBUG
				if (_Logging) {
					System.Diagnostics.POLYGONBOOLEAN_DEBUG.WriteLine(list[list.Count - 1].ToString() + " : " + (curIsRight ? edge.Right : edge.Left).Count);
				}
#endif

				// 指定側に１つでもポリゴンが存在すればポリゴンを形成できる
				if ((curIsRight ? edge.RightGroups : edge.LeftGroups).Count != 0)
					isNullInternal = false;

				// エッジのベクトルを計算
				var nextNodePos = nextNode.Position;
				var vec1 = curNode.Position - nextNodePos;
				var vec1v = vec1.VerticalCw();
				var vecLen1 = edge.Length;

				if (traceCCW)
					vec1v = -vec1v;

				// 次のエッジを探す
				// 指定方向のノードに接続されたエッジで且つエッジ同士のなす右回りの角度が最も小さい又は大きいものを選ぶ
				var nextEdges = nextNode.Edges;
				var nextEdgesLen = nextEdges.Count;
				Edge nextEdge = null;
				Node nextNextNode = null;
				var nextIsRight = true;
				var maxCos = element.MinValue;

				for (int i = nextEdgesLen - 1; i != -1; i--) {
					var e = nextEdges[i];
					if (e == edge)
						continue;
					var right = e.From == nextNode;
					if (e != startEdge) {
						if ((e.Flags & (right ? rightFlag : leftFlag)) != 0)
							continue;
					}
					var n2node = right ? e.To : e.From;
					var vec2 = n2node.Position - nextNodePos;
					var vecLen2 = e.Length;
					var cos = vec1.Dot(vec2) / (vecLen1 * vecLen2);
					if (0 <= vec1v.Dot(vec2)) {
						cos += 1;
					} else {
						cos = -1 - cos;
					}

					if (maxCos < cos) {
						nextEdge = e;
						nextNextNode = n2node;
						nextIsRight = right;
						maxCos = cos;
					}
				}

				if (nextEdge == startEdge) {
					if (list.Count < 3)
						isNullInternal = true;
					break;
				}
				if (nextEdge == null) {
#if POLYGONBOOLEAN_DEBUG
					if (_Logging) {
						System.Diagnostics.POLYGONBOOLEAN_DEBUG.WriteLine("ヒゲ");
					}
#endif
					isNullInternal = true;
					break;
				}

				// 次回の準備
				edge = nextEdge;
				curNode = nextNode;
				nextNode = nextNextNode;
				curIsRight = nextIsRight;
			}

			isNull = isNullInternal;

#if POLYGONBOOLEAN_DEBUG
			if (_Logging) {
				if (isNull)
					System.Diagnostics.POLYGONBOOLEAN_DEBUG.WriteLine("Is null");
			}
#endif

			return list;
		}

		/// <summary>
		/// AddPolygon() で追加されたポリゴンをノードとエッジに分解する
		/// </summary>
		private void MakeNodesAndEdges() {
			var nm = _NodeMgr;
			var em = _EdgeMgr;
			for (int groupIndex = 0, ngroups = _Groups.Count; groupIndex < ngroups; groupIndex++) {
				var group = _Groups[groupIndex];
				for (int polygonIndex = 0, npolygons = group.Count; polygonIndex < npolygons; polygonIndex++) {
					var pol = group[polygonIndex];
					var tpol = new TopologicalPolygon();

					{
						// 頂点をノードに変換する、半径 _Epsilon を使いノードの接触を調べ、接触しているなら既存のノードを使用する
						var vertices = pol.Vertices;
						var nnodes = vertices.Count;
						var nodes = new Node[nnodes];
						for (int i = 0, nvts = vertices.Count; i < nvts; i++) {
							var v = vertices[i];
							var node = _NodeMgr.New(v.Position);
							node.SetUserData(groupIndex, v.UserData);
							nodes[i] = node;
						}

						// ラインをエッジに変換する、既存エッジに同じノード組み合わせのものが存在したらそちらを使用する
						var edges = tpol.Edges = new List<EdgeAndSide>(nnodes);
						var node1 = nodes[0];
						for (int i = 1; i <= nnodes; i++) {
							var node2 = nodes[i % nnodes];
							var edge = _EdgeMgr.New(node1, node2);
							var right = node1 == edge.From;
							edge.LinkPolygon(right, groupIndex, polygonIndex);
							if (pol.EdgesUserData != null)
								edge.SetUserData(right, groupIndex, pol.EdgesUserData[i - 1]);
							edges.Add(new EdgeAndSide(edge, right));
							node1 = node2;
						}
					}

					// 穴を処理
					var holes = pol.Holes;
					if (holes != null) {
						tpol.Holes = new List<TopologicalPolygon>(holes.Count);

						for (int holeIndex = 0, nholes = holes.Count; holeIndex < nholes; holeIndex++) {
							var hole = holes[holeIndex];
							var holetpol = new TopologicalPolygon();

							// 頂点をノードに変換する、半径 _Epsilon を使いノードの接触を調べ、接触しているなら既存のノードを使用する
							var vertices = hole.Vertices;
							var nnodes = vertices.Count;
							var nodes = new Node[nnodes];
							for (int i = 0, nvts = vertices.Count; i < nvts; i++) {
								var v = vertices[i];
								var node = _NodeMgr.New(v.Position);
								node.SetUserData(groupIndex, v.UserData);
								nodes[i] = node;
							}

							// ラインをエッジに変換する、既存エッジに同じノード組み合わせのものが存在したらそちらを使用する
							var edges = holetpol.Edges = new List<EdgeAndSide>(nnodes);
							var node1 = nodes[0];
							for (int i = 1; i <= nnodes; i++) {
								var node2 = nodes[i % nnodes];
								var edge = _EdgeMgr.New(node1, node2);
								var right = node1 == edge.From;
								edge.LinkPolygon(right, groupIndex, polygonIndex);
								if (hole.EdgesUserData != null)
									edge.SetUserData(right, groupIndex, hole.EdgesUserData[i - 1]);
								edges.Add(new EdgeAndSide(edge, right));
								node1 = node2;
							}

							tpol.Holes.Add(holetpol);
						}
					}
					var tpols = new List<TopologicalPolygon>();
					tpols.Add(tpol);
					_TopoGroups.Add(tpols);
				}
				nm.Optimize();
				em.Optimize();
			}
		}

		/// <summary>
		/// エッジとノードの接触、エッジ同士の交点部分にノードを挿入する
		/// </summary>
		private void MakeIntersectionNodes() {
			// エッジ同士の交点を調べ、ノード挿入情報を作成する
			InsertIntersectionNodeToEdge();

			// 作成されたノード挿入情報を基にノードを挿入する
			EdgeDivide();
		}

		/// <summary>
		/// 全エッジの交差を調べ交差しているならエッジにノード挿入予約を行う
		/// </summary>
		private void InsertIntersectionNodeToEdge() {
			HashSet<ulong> comb = new HashSet<ulong>(); // 同じ組み合わせのチェックを排除するためのテーブル

			// 全エッジをチェックし、エッジ上にノードがあったら挿入予約を行う
			// 挿入予約を行ったノードから伸びるエッジは交差判定無視リストに登録する　※これを行わないと位相構造が壊れる
			var epsilon2 = _Epsilon * _Epsilon;
			var edges = this.Edges;
			foreach (var edge in this.Edges) {
				vector p, v;
				edge.GetLine(out p, out v);

				// エッジに接触する可能性があるノード探す
				foreach (var node in _NodeMgr.Query(edge.Volume)) {
					// ノードが現在処理中のエッジに繋がっていたらスキップ
					if (edge.From == node || edge.To == node)
						continue;

					// ノードとの線分最近点のパラメータを計算
					var t = LinePointNearestParam(p, v, node.Position);

					// 線分の範囲外ならスキップ
					if (t < 0 || 1 < t)
						continue;

					// 最近点とノードとの距離がノード半径を超えていたらスキップ
					var c = p + v * t;
					if (epsilon2 < (node.Position - c).LengthSquare)
						continue;

					// 挿入するノードとして登録
					node.Flags |= NodeFlags.OnEdge | NodeFlags.InsideOutside;
					edge.SetNodeInsertion(t, node);
					System.Diagnostics.Debug.WriteLine("On edge: " + edge + " " + node);

					// ノードにつながるエッジを交差判定無視リストに登録する
					var nodeEdges = node.Edges;
					for (int i = nodeEdges.Count - 1; i != -1; i--) {
						comb.Add(Edge.GetCombID(edge, nodeEdges[i]));
					}
				}
			}

			// エッジ同士の交差判定を行い、交差しているならノード挿入予約を行う
			var ing = _IntersectionNodeGenerator;
			foreach (var edge1 in this.Edges) {
				ulong uidx1 = edge1.UniqueIndex;

				// エッジから線分の情報取得
				vector p1, v1;
				edge1.GetLine(out p1, out v1);

				// エッジに接触する可能性があるエッジ探す
				foreach (var edge2 in _EdgeMgr.Query(edge1.Volume)) {
					ulong uidx2 = edge2.UniqueIndex;

					// すでにチェック済みの組み合わせならスキップ
					var combid = uidx1 <= uidx2 ? uidx2 << 32 | uidx1 : uidx1 << 32 | uidx2;
					if (comb.Contains(combid))
						continue;
					comb.Add(combid);

					// 位相構造上交差するはずが無いならスキップ
					if (!edge1.IsCrossable(edge2))
						continue;

					// エッジから線分の情報取得
					vector p2, v2;
					edge2.GetLine(out p2, out v2);

					// 交点のパラメータを計算
					element t1, t2;
					if (!Line2IntersectParamCheckRange(p1, v1, p2, v2, 0, out t1, out t2))
						continue;
					if (t1 == 0 || t1 == 1 || t2 == 0 || t2 == 1)
						continue;

					// 交点座標のノードを作成
					var node = _NodeMgr.New(p1 + v1 * t1);

					// ノードデータ生成デリゲートがあったら処理する
					if (ing != null)
						ing(edge1, t1, edge2, t2, node);

					// このノードは内外が入れ替わるノード
					// ノード挿入予約
					node.Flags |= NodeFlags.InsideOutside | NodeFlags.OnEdge;
					edge1.SetNodeInsertion(t1, node);
					edge2.SetNodeInsertion(t2, node);
					System.Diagnostics.Debug.WriteLine("Intersection: " + edge1 + "*" + edge2 + " " + node);
				}
			}
		}

		/// <summary>
		/// エッジのノード挿入情報を基にノードを挿入する
		/// </summary>
		private void EdgeDivide() {
			// 現時点での全エッジを対象に処理する
			foreach (var edge in new List<Edge>(this.Edges)) {
				var nis = edge.NodeInsertions;
				if (nis == null)
					continue;

				// 線分のパラメータで昇順にソート
				nis.Sort((a, b) => Math.Sign(a.Parameter - b.Parameter));

				// エッジを作成していく
				var node1 = edge.From;
				var count = nis.Count;
				for (int i = 0; i < count; i++) {
					var ni = nis[i];
					var newEdge = _EdgeMgr.New(node1, ni.Node);

					newEdge.CopyAttributes(edge, node1 == newEdge.From);

					node1 = ni.Node;
				}

				var newEdge2 = _EdgeMgr.New(node1, edge.To);
				newEdge2.CopyAttributes(edge, node1 == newEdge2.From);

				// ノード挿入情報をクリア
				edge.NodeInsertions = null;

				// 元のエッジ削除
				_EdgeMgr.Remove(edge);
			}
		}

		/// <summary>
		/// ヒゲを取り除く
		/// </summary>
		private void RemoveBeard() {
			List<Node> nodes = new List<Node>();
			var nm = _NodeMgr;
			var em = _EdgeMgr;
			for(;;) {
				// リンク数が１のノードを収集する
				nodes.AddRange(from n in this.Nodes where n.Edges.Count == 1 select n);
				if (nodes.Count == 0)
					break;

				// ノードとリンクしているエッジを取り除く
				for(int i = nodes.Count - 1; i != -1; i--) {
					var node = nodes[i];
					nm.Remove(node);
					if(node.Edges.Count != 0)
						em.Remove(node.Edges[0]);
				}
				nodes.Clear();
			}
		}

		/// <summary>
		/// ノードとエッジに分解後のデータを使い _TopologicalPolygons を再構成する
		/// </summary>
		/// <remarks>先に MakeNodesAndEdges()、MakeIntersectionNodes() 呼び出しておく必要がある</remarks>
		private void RebuildTpols() {
#if POLYGONBOOLEAN_DEBUG
			_Logging = true;
#endif
			for (int i = 0, n = _TopoGroups.Count; i < n; i++) {
				var groupIndex = i;

				// エッジの指定方向に指定ポリゴンが存在しないなら無視するフィルタ
				var edgeFilter = new Func<Edge, bool, bool>(
					(Edge e, bool right) => {
						List<int> r, l;
						if (right) {
							r = e.RightGroups;
							l = e.LeftGroups;
						} else {
							l = e.RightGroups;
							r = e.LeftGroups;
						}
						if (!r.Contains(groupIndex))
							return true;
						if (l.Contains(groupIndex))
							return true;
						return false;
					}
				);

				var groups = Distinguish(GetPolygons(edgeFilter, EdgeFlags.RightRemoved, EdgeFlags.LeftRemoved, false));
				var tpols = _TopoGroups[groupIndex];

				tpols.Clear();

				foreach (var loops in groups) {
					var tpol = new TopologicalPolygon();
					var nholes = loops.Count - 1;

					tpol.Edges = loops[0].Edges;
					if (nholes != 0) {
						var holes = tpol.Holes = new List<TopologicalPolygon>(nholes);
						for (int j = 1; j <= nholes; j++) {
							var holetpol = new TopologicalPolygon();
							holetpol.Edges = loops[j].Edges;
							holes.Add(holetpol);
						}
					}

					tpols.Add(tpol);
				}
			}
#if POLYGONBOOLEAN_DEBUG
			_Logging = false;
#endif
		}

		/// <summary>
		/// ノードとエッジに分解後のデータを使い _TopologicalPolygons 内ポリゴンエッジが他ポリゴンエッジに包含されているなら情報を付与する
		/// </summary>
		private void TpolsInclusionCheck() {
			var alltgroups = _TopoGroups;
			var nalltgroups = alltgroups.Count;

			// まず境界ボリュームと面積を計算する
			for (int groupIndex = nalltgroups - 1; groupIndex != -1; groupIndex--) {
				var tgroups = alltgroups[groupIndex];
				for (int i = tgroups.Count - 1; i != -1; i--) {
					var tpol = tgroups[i];
					var area = Area(tpol.Edges);
					tpol.Volume = new volume(from e in tpol.Edges select e.From.Position);
					tpol.Area = Math.Abs(area);
					tpol.CW = area <= 0;
					var holes = tpol.Holes;
					if (holes != null) {
						for (int holeIndex = holes.Count - 1; holeIndex != -1; holeIndex--) {
							var hole = holes[holeIndex];
							area = Area(hole.Edges);
							hole.Volume = new volume(from e in hole.Edges select e.From.Position);
							hole.Area = Math.Abs(area);
							hole.CW = area <= 0;
						}
					}
				}
			}

			// ポリゴンのエッジが他ポリゴンに完全に包含されているなら
			// エッジの両側に包含ポリゴンをリンクする
			// ※ポリゴンを構成するエッジが共有されているなら PolygonizeAll() によりリンクされるので必要ない
			for (int groupIndex1 = nalltgroups - 1; groupIndex1 != -1; groupIndex1--) {
				var tpols1 = alltgroups[groupIndex1];
				for (int i = tpols1.Count - 1; i != -1; i--) {
					var tpol1 = tpols1[i];

					for (int groupIndex2 = nalltgroups - 1; groupIndex2 != -1; groupIndex2--) {
						if (groupIndex2 == groupIndex1)
							continue;

						var tpols2 = alltgroups[groupIndex2];
						for (int j = tpols2.Count - 1; j != -1; j--) {
							tpol1.LinkPolygonIfContained(tpols2[j], groupIndex2, j);
						}
					}
				}
			}
		}

		/// <summary>
		/// 指定されたエッジによるポリゴン配列を外枠ポリゴンと穴に区別する
		/// </summary>
		/// <param name="edges">エッジによるポリゴン配列</param>
		/// <returns>外枠ポリゴン、穴、穴・・・の順に直した配列</returns>
		/// <remarks>外枠ポリゴンは時計回り、穴は反時計回りとなる</remarks>
		private static List<List<Loop>> Distinguish(List<List<EdgeAndSide>> edges) {
			// まず面積を求め、面積降順に並び替える
			var aes = new Loop[edges.Count];
			for (int i = 0, n = aes.Length; i < n; i++) {
				var e = edges[i];
				aes[i] = new Loop(Area(e), e);
			}
			Array.Sort(aes, (a, b) => Math.Sign(b.Area - a.Area));

			// 親子関係を調べる
			var parentIndices = new int[aes.Length];
			var nodeHashes = new HashSet<Node>[aes.Length];
			for (int i = aes.Length - 1; i != -1; i--) {
				// 子を取得
				var child = aes[i];
				var childVolume = child.Volume;
				var childEdges = child.Edges;

				// 親を探す
				parentIndices[i] = -1;
				for (int j = i - 1; j != -1; j--) {
					var parent = aes[j];

					if (!parent.Volume.Contains(childVolume))
						continue;

					// 親のノードがハッシュに登録されてなかったら登録する
					var parentEdges = parent.Edges;
					var parentNodes = nodeHashes[j];
					if (parentNodes == null) {
						nodeHashes[j] = parentNodes = new HashSet<Node>();
						for (int k = parentEdges.Count - 1; k != -1; k--) {
							parentNodes.Add(parentEdges[k].From);
						}
					}

					// 子と親が共用しているノードを1とし、それ以外を0とする
					// 0→1へ変化した際の0と1→0へ変化した際の0がポリゴンに包含されているか調べる
					// 0↔1の変化が無い場合には適当に選んだノードの包含を調べる
					var intersects = false;
					var parentContainsChild = false;
					var node1 = childEdges[0].From;
					var type1 = parentNodes.Contains(node1);
					for (int k = childEdges.Count - 1; k != -1; k--) {
						//if (type1 && (node1.Flags & NodeFlags.InsideOutside) != 0) {
						//	intersects = true;
						//	break; // TODO: 貫通フラグ見てるけど計算ポリゴン数２つだからこそできてる
						//}

						var node2 = childEdges[k].From;
						var type2 = parentNodes.Contains(node2);
						if (type1 != type2) {
							intersects = true;
							if (PointTouchPolygon2(type1 ? node2.Position : node1.Position, parentEdges, true) != 0) {
								parentContainsChild = true;
								break;
							}
						}
						node1 = node2;
						type1 = type2;
					}
					if (!intersects) {
						if (PointTouchPolygon2(node1.Position, parentEdges, true) != 0) {
							parentContainsChild = true;
						}
					}
					if (parentContainsChild) {
						parentIndices[i] = j;
						break;
					}
				}
			}

			// 親子関係を組んだリストを作成
			var result = new List<List<Loop>>();
			var used = new bool[aes.Length];
			for (int i = 0, n = aes.Length; i < n; i++) {
				if (used[i])
					continue;

				var list = new List<Loop>();

				// 親を取得
				var parent = aes[i];
				if (!parent.CW) {
					var parentEdges = parent.Edges;
					parentEdges.Reverse();
					for (int j = parentEdges.Count - 1; j != -1; j--) {
						var e = parentEdges[j];
						e.TraceRight = !e.TraceRight;
						parentEdges[j] = e;
					}
					parent.CW = true;
				}
				list.Add(parent);
				used[i] = true;

				// 子を取得
				for (int j = i + 1; j < n; j++) {
					if (used[j] || parentIndices[j] != i)
						continue;

					var child = aes[j];
					if (child.CW) {
						var childEdges = child.Edges;
						childEdges.Reverse();
						for (int k = childEdges.Count - 1; k != -1; k--) {
							var e = childEdges[k];
							e.TraceRight = !e.TraceRight;
							childEdges[k] = e;
						}
						child.CW = false;
					}
					list.Add(child);
					used[j] = true;
				}

				result.Add(list);
			}

			return result;
		}

		/// <summary>
		/// pを開始点としvを方向ベクトルとする線分と点cとの最近点の線分パラメータを計算する
		/// </summary>
		/// <param name="p">[in] 線分の開始点</param>
		/// <param name="v">[in] 線分の方向ベクトル</param>
		/// <param name="c">[in] 最近点を調べる点c</param>
		/// <returns>線分のパラメータ</returns>
		private static element LinePointNearestParam(vector p, vector v, vector c) {
			return (c - p).Dot(v) / v.LengthSquare;
		}

		/// <summary>
		/// ２次元線分同士が交差しているか調べる、交点のパラメータは計算しない（整数ベクトル使用可）
		/// </summary>
		/// <param name="s1">[in] 線分１の開始点</param>
		/// <param name="e1">[in] 線分１の終了点</param>
		/// <param name="s2">[in] 線分２の開始点</param>
		/// <param name="e2">[in] 線分２の終了点</param>
		/// <returns>交差しているなら true が返る</returns>
		private static bool LineIntersect(vector s1, vector e1, vector s2, vector e2) {
			var v = s1 - e1;
			var ox = s2.Y - s1.Y;
			var oy = s1.X - s2.X;
			if (0 <= (v.X * ox + v.Y * oy) * (v.X * (e2.Y - s1.Y) + v.Y * (s1.X - e2.X)))
				return false;
			v = s2 - e2;
			if (0 <= -(v.X * ox + v.Y * oy) * (v.X * (e1.Y - s2.Y) + v.Y * (s2.X - e1.X)))
				return false;
			return true;
		}

		/// <summary>
		/// ２次元線分同士の交点パラメータを計算する
		/// </summary>
		/// <param name="p1">[in] 線分１の開始点</param>
		/// <param name="v1">[in] 線分１の方向ベクトル</param>
		/// <param name="p2">[in] 線分２の開始点</param>
		/// <param name="v2">[in] 線分２の方向ベクトル</param>
		/// <param name="t1">[out] 線分１のパラメータが返る</param>
		/// <param name="t2">[out] 線分２のパラメータが返る</param>
		/// <returns>計算できたら true が返る</returns>
		private static bool Line2IntersectParam(vector p1, vector v1, vector p2, vector v2, out element t1, out element t2) {
			var d = v1.X * v2.Y - v1.Y * v2.X;
			if (d == 0) {
				t1 = t2 = 0;
				return false;
			}
			var pv = p2 - p1;
			t1 = (pv.X * v2.Y - pv.Y * v2.X) / d;
			t2 = (pv.X * v1.Y - pv.Y * v1.X) / d;
			return true;
		}

		/// <summary>
		/// ２次元線分同士の交点パラメータを計算する、その際に線分範囲外が判明し次第計算を打ち切る
		/// </summary>
		/// <param name="p1">[in] 線分１の開始点</param>
		/// <param name="v1">[in] 線分１の方向ベクトル</param>
		/// <param name="p2">[in] 線分２の開始点</param>
		/// <param name="v2">[in] 線分２の方向ベクトル</param>
		/// <param name="tolerance">[in] 線分パラメータ範囲内判定用許容誤差値、許容誤差内なら0～1の範囲を超えていても交差していることにする</param>
		/// <param name="t1">[out] 線分１のパラメータが返る</param>
		/// <param name="t2">[out] 線分２のパラメータが返る</param>
		/// <returns>交差するなら true が返る</returns>
		private static bool Line2IntersectParamCheckRange(vector p1, vector v1, vector p2, vector v2, element tolerance, out element t1, out element t2) {
			var d = v1.X * v2.Y - v1.Y * v2.X;
			if (d == 0) {
				t1 = t2 = 0;
				return false;
			}
			var pv = p2 - p1;
			t2 = (pv.X * v1.Y - pv.Y * v1.X) / d;
			if (t2 < -tolerance || tolerance < t2 - 1) {
				t1 = 0;
				return false;
			}
			t1 = (pv.X * v2.Y - pv.Y * v2.X) / d;
			return -tolerance <= t1 && t1 - 1 <= tolerance;
		}

		/// <summary>
		/// 多角形の面積と回り方向を計算する
		/// </summary>
		/// <param name="vertices">[in] 多角形の頂点列</param>
		/// <returns>多角形の面積が返る、ポリゴンが反時計回りなら正数、時計回りなら負数となる</returns>
		private static element Area(List<Vertex> vertices) {
			var nvts = vertices.Count;
			int i, n = nvts + 1;
			var p1 = vertices[0].Position;
			element s = 0;
			for (i = 1; i < n; i++) {
				var p2 = vertices[i % nvts].Position;
				s += (p1.X - p2.X) * (p1.Y + p2.Y);
				p1 = p2;
			}
			return s / 2;
		}

		/// <summary>
		/// 多角形の面積と回り方向を計算する
		/// </summary>
		/// <param name="edges">[in] 多角形の頂点列</param>
		/// <returns>多角形の面積が返る、ポリゴンが反時計回りなら正数、時計回りなら負数となる</returns>
		private static element Area(List<EdgeAndSide> edges) {
			var nvts = edges.Count;
			int i, n = nvts + 1;
			var p1 = edges[0].From.Position;
			element s = 0;
			for (i = 1; i < n; i++) {
				var p2 = edges[i % nvts].From.Position;
				s += (p1.X - p2.X) * (p1.Y + p2.Y);
				p1 = p2;
			}
			return s / 2;
		}

		/// <summary>
		/// 点が２次元多角形の内にあるか調べる、辺と点上の座標は接触しているとみなされない
		/// </summary>
		/// <param name="c">[in] 点の座標</param>
		/// <param name="vts">[in] 多角形の頂点配列</param>
		/// <param name="close">[in] 多角形の始点と終点を閉じて判定をする場合はtrueを指定する</param>
		/// <returns>点が多角形内にあるならtrueが返る</returns>
		private static bool PointInPolygon2(vector c, List<Vertex> vts, bool close = false) {
			var nvts = vts.Count;
			int i, count = 0;
			int n = close ? nvts + 1 : nvts;
			var p1 = vts[0].Position;
			var x1 = c.X - p1.X;
			element zero = 0;
			for (i = 1; i < n; i++) {
				var p2 = vts[close ? i % nvts : i].Position;
				var x2 = c.X - p2.X;
				if ((x1 < zero && zero <= x2) || (zero <= x1 && x2 < zero))
					count += x1 * (p2.Y - p1.Y) < (c.Y - p1.Y) * (p2.X - p1.X) ? -1 : 1;
				p1 = p2;
				x1 = x2;
			}
			return count != 0;
		}

		/// <summary>
		/// 点が２次元多角形の内にあるか調べる、辺と点上の座標は接触しているとみなされない
		/// </summary>
		/// <param name="c">[in] 点の座標</param>
		/// <param name="edges">[in] 多角形のエッジ配列</param>
		/// <param name="close">[in] 多角形の始点と終点を閉じて判定をする場合はtrueを指定する</param>
		/// <returns>点が多角形内にあるならtrueが返る</returns>
		private static bool PointInPolygon2(vector c, List<EdgeAndSide> edges, bool close = false) {
			var nvts = edges.Count;
			int i, count = 0;
			int n = close ? nvts + 1 : nvts;
			var p1 = edges[0].From.Position;
			var x1 = c.X - p1.X;
			element zero = 0;
			for (i = 1; i < n; i++) {
				var p2 = edges[close ? i % nvts : i].From.Position;
				var x2 = c.X - p2.X;
				if ((x1 < zero && zero <= x2) || (zero <= x1 && x2 < zero))
					count += x1 * (p2.Y - p1.Y) < (c.Y - p1.Y) * (p2.X - p1.X) ? -1 : 1;
				p1 = p2;
				x1 = x2;
			}
			return count != 0;
		}

		/// <summary>
		/// 点が２次元多角形に接触しているか調べる、辺と点上の座標は接触しているとみなす
		/// </summary>
		/// <param name="c">[in] 点の座標</param>
		/// <param name="edges">[in] 多角形のエッジ配列</param>
		/// <param name="close">[in] 多角形の始点と終点を閉じて判定をする場合は０以外を指定する</param>
		/// <returns>点が多角形内にあるなら2、辺または頂点上にあるなら1、それ以外なら0が返る</returns>
		private static int PointTouchPolygon2(vector c, List<EdgeAndSide> edges, bool close = false) {
			var nvts = edges.Count;
			int i, count = 0;
			int n = close ? nvts + 1 : nvts;
			var p1 = edges[0].From.Position;
			var y1 = c.Y - p1.Y;
			element zero = 0;
			for (i = 1; i < n; i++) {
				var p2 = edges[close ? i % nvts : i].From.Position;
				var y2 = c.Y - p2.Y;
				if ((y1 < zero && zero <= y2) || (zero <= y1 && y2 < zero)) {
					var rx = c.X - p1.X;
					var dy = p2.Y - p1.Y;
					var dx = p2.X - p1.X;
					var t1 = y1 * dx;
					var t2 = rx * dy;
					if (t1 == t2)
						return 1;
					count += t1 < t2 ? -1 : 1;
				} else if (y1 == zero && y2 == zero) {
					var x1 = c.X - p1.X;
					var x2 = c.X - p2.X;
					if ((x1 <= zero) != (x2 <= zero) || x1 == zero || x2 == zero)
						return 1;
				} else if (c == p1) {
					return 1;
				}
				p1 = p2;
				y1 = y2;
			}
			return count != 0 ? 2 : 0;
		}
		#endregion
	}
}
