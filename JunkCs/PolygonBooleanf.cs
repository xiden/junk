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
	public class PolygonBooleanf {
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
			/// コンストラクタ、頂点座標配列を渡して検索用インデックスを作成する
			/// </summary>
			/// <param name="vertices">頂点配列</param>
			public Polygon(List<Vertex> vertices) {
				this.Vertices = vertices;
			}

			/// <summary>
			/// コンストラクタ、頂点座標配列を渡して検索用インデックスを作成する
			/// </summary>
			/// <param name="vertices">頂点配列</param>
			public Polygon(IEnumerable<Vertex> vertices) {
				this.Vertices = new List<Vertex>(vertices);
			}

			/// <summary>
			/// ポリゴンがブーリアン演算の入力として使えるか調べる
			/// </summary>
			/// <param name="epsilon">頂点半径</param>
			/// <returns>使えるなら true が返る</returns>
			public ValidationResult Validation(element epsilon) {
				var vertices = this.Vertices;
				var nvts = vertices.Count;

				var VFinder = new VertexFinder(vertices, epsilon);
				var LFinder = new LineFinder(vertices);

				// 最低でも３点以上ないとだめ
				if (nvts < 3)
					return new ValidationResult(false, "ポリゴンの頂点数は３以上でなければなりません。");

				// 時計回りチェック
				if (0 <= Area((from r in vertices select r.Position).ToArray()))
					return new ValidationResult(false, "ポリゴンは時計回りでなければなりません。");

				// 同一頂点座標チェック
				var epsilon2 = epsilon * epsilon;
				var vfinder = VFinder;
				for (int i = 0; i < nvts; i++) {
					var v = vertices[i].Position;
					var indices = vfinder.Query(new volume(v - epsilon, v + epsilon));

					foreach (var j in indices) {
						if (j != i && (vertices[j].Position - v).LengthSquare <= epsilon2)
							return new ValidationResult(false, "ポリゴンは同一座標の頂点が複数あってはなりません。");
					}
				}

				// 辺同士の交差チェック
				var s1 = vertices[0].Position;
				var lfinder = LFinder;
				for (int i = 1, n = nvts + 1; i < n; i++) {
					var index = i - 1;
					var indexp1 = (index + 1) % nvts;
					var indexm1 = (index - 1 + nvts) % nvts;
					var e1 = vertices[i % nvts].Position;
					var volume = new volume(s1, e1, true).Expand(epsilon);
					var indices = lfinder.Query(volume);

					foreach (var j in indices) {
						if (j != index && j != indexp1 && j != indexm1) {
							var s2 = vertices[j].Position;
							var e2 = vertices[(j + 1) % nvts].Position;
							if (LineIntersect(s1, e1, s2, e2))
								return new ValidationResult(false, "ポリゴンは自己交差があってはなりません。");
						}
					}

					s1 = e1;
				}

				return new ValidationResult(true, "有効なポリゴンです。ブーリアン処理に使用できます。");
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
			/// 使用されているノード
			/// </summary>
			Used = 1 << 1,

			/// <summary>
			/// エッジ上に挿入されるノード
			/// </summary>
			OnEdge = 1 << 2,
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
			/// 座標
			/// </summary>
			public vector Position;

			/// <summary>
			/// ユーザーデータ
			/// </summary>
			public Dictionary<int, object> UserData;

			/// <summary>
			/// このノードに接続されているエッジ一覧
			/// </summary>
			public List<Edge> Edges = new List<Edge>();

			/// <summary>
			/// 指定のエッジへのリンクを追加する
			/// </summary>
			/// <param name="edge">エッジ</param>
			public void Add(Edge edge) {
				if (this.Edges.Contains(edge))
					return;
				this.Edges.Add(edge);
				this.Flags |= NodeFlags.Used;
			}

			/// <summary>
			/// 指定のエッジへのリンクを削除する
			/// </summary>
			/// <param name="edge">エッジ</param>
			public void Remove(Edge edge) {
				if (!this.Edges.Contains(edge))
					return;
				this.Edges.Remove(edge);
				if(this.Edges.Count == 0)
					this.Flags &= ~NodeFlags.Used;
			}

			/// <summary>
			/// 指定ポリゴン用のユーザーデータを設定する
			/// </summary>
			/// <param name="polygonIndex">ポリゴンインデックス</param>
			/// <param name="userData">ユーザーデータ</param>
			public void SetUserData(int polygonIndex, object userData) {
				if (this.UserData == null)
					this.UserData = new Dictionary<int, object>();
				this.UserData[polygonIndex] = userData;
			}

			/// <summary>
			/// 指定ポリゴン用のユーザーデータを取得する
			/// </summary>
			/// <param name="polygonIndex">ポリゴンインデックス</param>
			/// <returns>ユーザーデータ又は null</returns>
			public object GetUserData(int polygonIndex) {
				if (this.UserData == null)
					return null;
				object obj;
				if (this.UserData.TryGetValue(polygonIndex, out obj))
					return obj;
				else
					return null;
			}

			/// <summary>
			/// 指定されたポリゴンがリンクされているかどうか調べる
			/// </summary>
			/// <param name="polygonIndex">ポリゴンインデックス</param>
			/// <returns>リンクされているなら true</returns>
			public bool IsLinked(int polygonIndex) {
				foreach(var edge in this.Edges) {
					if (edge.IsLinked(polygonIndex))
						return true;
				}
				return false;
			}

			public static bool operator <(Node a, Node b) {
				if (a.Position.X < b.Position.X)
					return true;
				if (a.Position.X == b.Position.X)
					return a.Position.Y < b.Position.Y;
				else
					return false;
			}

			public static bool operator >(Node a, Node b) {
				if (a.Position.X > b.Position.X)
					return true;
				if (a.Position.X == b.Position.X)
					return a.Position.Y > b.Position.Y;
				else
					return false;
			}
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
			/// 進行方向From→Toの右側のポリゴンインデックス一覧
			/// </summary>
			public List<int> Right = new List<int>();

			/// <summary>
			/// 進行方向From→Toの左側のポリゴンインデックス一覧
			/// </summary>
			public List<int> Left = new List<int>();

			/// <summary>
			/// null でない場合にはエッジ上にノードを挿入する予定であることを示す
			/// </summary>
			public List<NodeInsertion> NodeInsertions;

			/// <summary>
			/// エッジ識別子の取得
			/// </summary>
			public EdgeID ID {
				get {
					return new EdgeID(this.From, this.To);
				}
			}

			/// <summary>
			/// エッジの境界ボリュームの取得
			/// </summary>
			public volume Volume {
				get {
					return new volume(this.From.Position, this.To.Position, true);
				}
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
				from.Add(this);
				to.Add(this);
			}

			/// <summary>
			/// ノードから切断する
			/// </summary>
			public void Disconnect() {
				if(this.From != null) {
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
			/// <param name="polygonIndex">ポリゴンインデックス</param>
			public void LinkPolygon(bool right, int polygonIndex) {
				var list = right ? this.Right : this.Left;
				if (list.Contains(polygonIndex))
					return;
				list.Add(polygonIndex);
			}

			/// <summary>
			/// ポリゴンへリンクする
			/// </summary>
			/// <param name="from">ポリゴン内でのエッジの開始ノード</param>
			/// <param name="polygonIndex">ポリゴンインデックス</param>
			public void LinkPolygon(Node from, int polygonIndex) {
				LinkPolygon(from == this.From, polygonIndex);
			}

			/// <summary>
			/// 指定エッジがリンクしているポリゴンを方向を考慮しつつリンクする
			/// </summary>
			/// <param name="edge">エッジ</param>
			/// <param name="sameDir">指定エッジが同じ方向かどうか</param>
			public void LinkPolygon(Edge edge, bool sameDir) {
				List<int> r, l;
				if (sameDir) {
					r = this.Right;
					l = this.Left;
				} else {
					l = this.Right;
					r = this.Left;
				}
				foreach (var p in edge.Right) {
					if (!r.Contains(p))
						r.Add(p);
				}
				foreach (var p in edge.Left) {
					if (!l.Contains(p))
						l.Add(p);
				}
			}

			/// <summary>
			/// 指定されたポリゴンがリンクされているかどうか調べる
			/// </summary>
			/// <param name="polygonIndex">ポリゴンインデックス</param>
			/// <returns>リンクされているなら true</returns>
			public bool IsLinked(int polygonIndex) {
				return this.Right.Contains(polygonIndex) || this.Left.Contains(polygonIndex);
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
				if(from == this.From) {
					p = this.From.Position;
					v = this.To.Position - p;
				} else if(from == this.To) {
					p = this.To.Position;
					v = this.From.Position - p;
				} else {
					throw new ApplicationException();
				}
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

				foreach (var p in this.Right)
					set1.Add(p);
				foreach (var p in this.Left)
					set1.Add(p);
				foreach (var p in edge.Right)
					set2.Add(p);
				foreach (var p in edge.Left)
					set2.Add(p);

				foreach(var p in set1) {
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
		}

		/// <summary>
		/// 構造が同一のエッジを識別するための構造体
		/// </summary>
		public struct EdgeID {
			/// <summary>
			/// 開始ノード
			/// </summary>
			public Node From;

			/// <summary>
			/// 終了ノード
			/// </summary>
			public Node To;

			public EdgeID(Node from, Node to) {
				this.From = from;
				this.To = to;
			}

			public override int GetHashCode() {
				return (this.From != null ? this.From.GetHashCode() : 0 ) ^ (this.To != null ? this.To.GetHashCode() : 0);
			}

			public override bool Equals(object obj) {
				if (obj is EdgeID)
					return this == (EdgeID)obj;
				else
					return false;
			}

			public static bool operator ==(EdgeID e1, EdgeID e2) {
				var f1 = e1.From;
				var t1 = e1.To;
				var f2 = e2.From;
				var t2 = e2.To;
				if (t1 < f1) {
					var n = f1;
					f1 = t1;
					t1 = n;
				}
				if (t2 < f2) {
					var n = f2;
					f2 = t2;
					t2 = n;
				}
				return f1 == f2 && t1 == t2;
			}

			public static bool operator !=(EdgeID e1, EdgeID e2) {
				var f1 = e1.From;
				var t1 = e1.To;
				var f2 = e2.From;
				var t2 = e2.To;
				if (t1 < f1) {
					var n = f1;
					f1 = t1;
					t1 = n;
				}
				if (t2 < f2) {
					var n = f2;
					f2 = t2;
					t2 = n;
				}
				return f1 != f2 || t1 != t2;
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

		/// <summary>
		/// 座標の範囲で頂点を検索するヘルパクラス
		/// </summary>
		public class VertexFinder {
			public DynamicAABB2fTree<int> _Tree = new DynamicAABB2fTree<int>();

			/// <summary>
			/// コンストラクタ、頂点座標配列を渡して検索用インデックスを作成する
			/// </summary>
			/// <param name="vertices">頂点配列</param>
			/// <param name="epsilon">頂点の半径</param>
			public VertexFinder(List<Vertex> vertices, element epsilon) {
				var tree = _Tree;
				for (int i = vertices.Count - 1; i != -1; i--) {
					var v = vertices[i].Position;
					tree.Add(new volume(v - epsilon, v + epsilon), i);
				}
				tree.OptimizeTopDown();
			}

			/// <summary>
			/// 指定範囲内の頂点を検索しインデックス番号配列を取得する
			/// </summary>
			/// <param name="volume">検索範囲を表す境界ボリューム</param>
			/// <returns>範囲内の頂点インデックス番号配列</returns>
			public List<int> Query(volume volume) {
				return new List<int>(_Tree.Query(volume));
			}
		}

		/// <summary>
		/// 座標の範囲で頂点間のラインを検索するヘルパクラス
		/// </summary>
		public class LineFinder {
			public DynamicAABB2fTree<int> _Tree = new DynamicAABB2fTree<int>();

			/// <summary>
			/// コンストラクタ、頂点座標配列を渡して検索用インデックスを作成する
			/// </summary>
			/// <param name="vertices">頂点配列</param>
			public LineFinder(List<Vertex> vertices) {
				if (vertices.Count < 2)
					return;
				var tree = _Tree;
				var nvts = vertices.Count;
				var v1 = vertices[0].Position;
				for (int i = 1, n = nvts + 1; i < n; i++) {
					var v2 = vertices[i % nvts].Position;
					var j = i - 1;
					var volume = new volume(v1, v2, true);

					tree.Add(volume, j);

					v1 = v2;
				}
				tree.OptimizeTopDown();
			}

			/// <summary>
			/// 指定範囲内の頂点を検索しインデックス番号配列を取得する
			/// </summary>
			/// <param name="volume">検索範囲を表す境界ボリューム</param>
			/// <returns>範囲内の頂点インデックス番号配列</returns>
			public List<int> Query(volume volume) {
				return new List<int>(_Tree.Query(volume));
			}
		}

		/// <summary>
		/// ノードとエッジで構成されるポリゴン
		/// </summary>
		public class TopologicalPolygon {
			/// <summary>
			/// ノード列
			/// </summary>
			public List<Node> Nodes = new List<Node>();

			/// <summary>
			/// エッジ列
			/// </summary>
			public List<Edge> Edges = new List<Edge>();
		}

		/// <summary>
		/// ノード管理クラス
		/// </summary>
		public class NodeManager {
			List<Node> _Nodes = new List<Node>();
			DynamicAABB2fTree<Node> _Tree = new DynamicAABB2fTree<Node>();
			element _Epsilon;

			/// <summary>
			/// 全ノード一覧の取得
			/// </summary>
			public ICollection<Node> AllNodes {
				get {
					return _Nodes.AsReadOnly();
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
			public Node NewNode(vector position) {
				// ノードの境界ボリューム計算
				var volume = new AABB2f(position).Expand(_Epsilon);

				// 境界ボリューム同士が接触するノード一覧取得
				var list = new List<Node>(_Tree.Query(volume));

				// ノード一覧内で最も距離が近いものを探す
				Node node = null;
				var mindist2 = element.MaxValue;
				var epsilon2 = _Epsilon * _Epsilon;
				for (int i = 0, n = list.Count; i < n; i++) {
					var nd = list[i];
					var dist2 = (position - nd.Position).LengthSquare;
					if (dist2 <= epsilon2 && dist2 < mindist2) {
						node = list[i];
						mindist2 = dist2;
					}
				}

				// 接触しているノードが無かった場合のみ新規作成
				if (node == null) {
					node = new Node { Position = position };
					_Nodes.Add(node);
					_Tree.Add(volume, node);
				}

				return node;
			}

			/// <summary>
			/// 指定された境界ボリュームに接触するノード一覧取得
			/// </summary>
			/// <param name="volume">境界ボリューム</param>
			/// <returns>ノード一覧</returns>
			public IEnumerable<Node> Query(volume volume) {
				return _Tree.Query(volume);
			}
		}

		/// <summary>
		/// エッジ管理クラス
		/// </summary>
		public class EdgeManager {
			uint _UniqueIndex;
			Dictionary<EdgeID, Edge> _Edges = new Dictionary<EdgeID, Edge>();
			DynamicAABB2fTree<Edge> _Tree = new DynamicAABB2fTree<Edge>();
			element _Epsilon;

			/// <summary>
			/// 全エッジ一覧の取得
			/// </summary>
			public ICollection<Edge> AllEdges {
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
			public Edge NewEdge(Node from, Node to) {
				var id = new EdgeID(from, to);
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
			public void RemoveEdge(Edge edge) {
				if (!_Edges.ContainsKey(edge.ID))
					return;
				_Edges.Remove(edge.ID);

				var volume = edge.Volume.Expand(_Epsilon);
				var leaf = (from r in _Tree.QueryLeaves(volume) where r.Data == edge select r).FirstOrDefault();
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
			public bool Right;

			public EdgeAndSide(Edge edge, bool right) {
				this.Edge = edge;
				this.Right = right;
			}

			public Node From {
				get {
					return this.Right ? this.Edge.From : this.Edge.To;
				}
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
		#endregion

		#region フィールド
		NodeManager _NodeMgr;
		EdgeManager _EdgeMgr;
		List<Polygon> _Polygons = new List<Polygon>();
		List<TopologicalPolygon> _TopologicalPolygons = new List<TopologicalPolygon>();
		element _Epsilon;
		bool _RemoveFlagsIsSet;
		#endregion

		#region プロパティ
		/// <summary>
		/// ノード一覧の取得
		/// </summary>
		public ICollection<Node> Nodes {
			get {
				return _NodeMgr.AllNodes;
			}
		}

		/// <summary>
		/// エッジ一覧の取得
		/// </summary>
		public ICollection<Edge> Edges {
			get {
				return _EdgeMgr.AllEdges;
			}
		}
		#endregion

		#region 公開メソッド
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="epsilon">頂点半径、接触判定時に使用する</param>
		public PolygonBooleanf(element epsilon) {
			_NodeMgr = new NodeManager(epsilon);
			_EdgeMgr = new EdgeManager(epsilon);
			_Epsilon = epsilon;
		}

		/// <summary>
		/// ポリゴンを登録する
		/// </summary>
		/// <param name="vertices">ポリゴンの頂点列</param>
		/// <returns>追加されたポリゴンのインデックス</returns>
		public int AddPolygon(IEnumerable<Vertex> vertices) {
			return AddPolygon(new Polygon(new List<Vertex>(vertices)));
		}

		/// <summary>
		/// ポリゴンを登録する
		/// </summary>
		/// <param name="polygon">ポリゴン</param>
		/// <returns>追加されたポリゴンのインデックス</returns>
		public int AddPolygon(Polygon polygon) {
			var result = _Polygons.Count;
			_Polygons.Add(polygon);
			return result;
		}

		/// <summary>
		/// ポリゴンをノードとエッジに分解する、ポリゴン同士の交点にはノードが追加される
		/// </summary>
		/// <returns>全てのポリゴンを構成するエッジと方向の一覧</returns>
		public List<List<EdgeAndSide>> CreateTopology() {
			// ポリゴンが使用可能か調べる
			var epsilon = _Epsilon;
			for(int i = 0, n = _Polygons.Count; i < n; i++) {
				var pol = _Polygons[i];
				var result = pol.Validation(epsilon);
				if(!result.IsValid) {
					throw new ApplicationException("ポリゴン" + (i + 1) + ": " + result.Message);
				}
			}

			// ポリゴンからノードとエッジを作成する
			MakeNodesAndEdges();

			// 交点にノードを挿入する
			MakeIntersectionNodes();

			// ポリゴンを構成する
			return PolygonizeAll();
		}

		/// <summary>
		/// ポリゴンを構成するエッジと方向の一覧からポリゴンを生成する
		/// </summary>
		/// <param name="edges">エッジと方向の一覧</param>
		/// <returns>ポリゴン</returns>
		public static Polygon PolygonFromEdges(IEnumerable<EdgeAndSide> edges) {
			return new Polygon(from r in edges let node = r.From select new Vertex(node.Position, node.UserData));
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
			for(int i = 0; i < n; i++) {
				var e = edges[i];
				if(!matcher(e.Edge, e.Right)) {
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
				var match = matcher(e.Edge, e.Right);
				if(match) {
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
		/// AddPolygon() で登録されたポリゴン同士のORポリゴンを作成する
		/// </summary>
		/// <returns>結果のポリゴンを構成するエッジと方向の一覧</returns>
		/// <remarks>事前に CreateTopology() を呼び出しておく必要がある。</remarks>
		public List<List<EdgeAndSide>> Or() {
			BeginSetRemoveFlags();

			var polygons = new List<List<EdgeAndSide>>();

			// エッジの両側にポリゴンが存在するなら無視するフィルタ
			var edgeFilter = new Func<Edge, bool, bool>(
				(Edge e, bool right) => {
					return e.Left.Count != 0 && e.Right.Count != 0;
				}
			);

			// 予め無視することがわかっているエッジを処理
			foreach (var edge in this.Edges) {
				if (edgeFilter(edge, true))
					edge.Flags |= EdgeFlags.RightRemoved;
			}

			// 右側にまだポリゴンが存在するエッジのみ処理する
			foreach (var edge in this.Edges) {
				if ((edge.Flags & EdgeFlags.RightRemoved) != 0)
					continue;

				// ポリゴンを構成するエッジと方向一覧を取得
				var edges = RightPolygon(edge, edgeFilter);

				// エッジの指定方向を処理済みとする
				foreach(var eas in edges) {
					eas.Edge.Flags |= eas.Right ? EdgeFlags.RightRemoved : EdgeFlags.LeftRemoved;
				}

				// 結果のポリゴン一覧に追加
				polygons.Add(edges);
			}

			return polygons;
		}

		/// <summary>
		/// AddPolygon() で登録されたポリゴン同士のXORポリゴンを作成する
		/// </summary>
		/// <returns>結果のポリゴンを構成するエッジと方向の一覧</returns>
		/// <remarks>事前に CreateTopology() を呼び出しておく必要がある。</remarks>
		public List<List<EdgeAndSide>> Xor() {
			BeginSetRemoveFlags();

			var polygons = new List<List<EdgeAndSide>>();

			// エッジの指定方向に複数ポリゴンが存在するなら無視するフィルタ
			var edgeFilter = new Func<Edge, bool, bool>(
				(Edge e, bool right) => {
					return (right ? e.Right : e.Left).Count != 1;
				}
			);

			// 予め無視することがわかっているエッジを処理
			foreach (var edge in this.Edges) {
				if (edgeFilter(edge, true))
					edge.Flags |= EdgeFlags.RightRemoved;
				if (edgeFilter(edge, false))
					edge.Flags |= EdgeFlags.LeftRemoved;
			}

			// 右側にまだポリゴンが存在するエッジのみ処理する
			foreach (var edge in this.Edges) {
				if ((edge.Flags & EdgeFlags.RightRemoved) != 0)
					continue;

				// ポリゴンを構成するエッジと方向一覧を取得
				var edges = RightPolygon(edge, edgeFilter);

				// エッジの指定方向を処理済みとする
				foreach (var eas in edges) {
					eas.Edge.Flags |= eas.Right ? EdgeFlags.RightRemoved : EdgeFlags.LeftRemoved;
				}

				// 結果のポリゴン一覧に追加
				polygons.Add(edges);
			}

			return polygons;
		}

		/// <summary>
		/// AddPolygon() で登録されたポリゴン同士のANDポリゴンを作成する
		/// </summary>
		/// <returns>結果のポリゴンを構成するエッジと方向の一覧</returns>
		/// <remarks>事前に CreateTopology() を呼び出しておく必要がある。</remarks>
		public List<List<EdgeAndSide>> And() {
			BeginSetRemoveFlags();

			var polygons = new List<List<EdgeAndSide>>();
			var allpols = new int[_TopologicalPolygons.Count];

			for (int i = 0, n = allpols.Length; i < n; i++)
				allpols[i] = i;

			// エッジの指定方向に登録ポリゴンの内一つでも存在しないなら無視するフィルタ
			var edgeFilter = new Func<Edge, bool, bool>(
				(Edge e, bool right) => {
					var pols = right ? e.Right : e.Left;
					foreach(var p in allpols) {
						if (!pols.Contains(p))
							return true;
					}
					return false;
				}
			);

			// 予め無視することがわかっているエッジを処理
			foreach (var edge in this.Edges) {
				if (edgeFilter(edge, true))
					edge.Flags |= EdgeFlags.RightRemoved;
				if (edgeFilter(edge, false))
					edge.Flags |= EdgeFlags.LeftRemoved;
			}

			// 右側にまだポリゴンが存在するエッジのみ処理する
			foreach (var edge in this.Edges) {
				if ((edge.Flags & EdgeFlags.RightRemoved) != 0)
					continue;

				// ポリゴンを構成するエッジと方向一覧を取得
				var edges = RightPolygon(edge, edgeFilter);

				// エッジの指定方向を処理済みとする
				foreach (var eas in edges) {
					eas.Edge.Flags |= eas.Right ? EdgeFlags.RightRemoved : EdgeFlags.LeftRemoved;
				}

				// 結果のポリゴン一覧に追加
				polygons.Add(edges);
			}

			return polygons;
		}

		/// <summary>
		/// 指定されたインデックスのポリゴンを減算したポリゴンを作成する
		/// </summary>
		/// <param name="polygonIndex">減算するポリゴンのインデックス</param>
		/// <returns>結果のポリゴンを構成するエッジと方向の一覧</returns>
		/// <remarks>事前に CreateTopology() を呼び出しておく必要がある。</remarks>
		public List<List<EdgeAndSide>> Sub(int polygonIndex) {
			BeginSetRemoveFlags();

			var polygons = new List<List<EdgeAndSide>>();

			// エッジの指定方向に減算ポリゴンが存在するなら無視するフィルタ
			var edgeFilter = new Func<Edge, bool, bool>(
				(Edge e, bool right) => {
					return (right ? e.Right : e.Left).Contains(polygonIndex);
				}
			);

			// 予め無視することがわかっているエッジを処理
			foreach (var edge in this.Edges) {
				if (edgeFilter(edge, true))
					edge.Flags |= EdgeFlags.RightRemoved;
				if (edgeFilter(edge, false))
					edge.Flags |= EdgeFlags.LeftRemoved;
			}

			// 右側にまだポリゴンが存在するエッジのみ処理する
			foreach (var edge in this.Edges) {
				if ((edge.Flags & EdgeFlags.RightRemoved) != 0)
					continue;

				// ポリゴンを構成するエッジと方向一覧を取得
				var edges = RightPolygon(edge, edgeFilter);

				// エッジの指定方向を処理済みとする
				foreach (var eas in edges) {
					eas.Edge.Flags |= eas.Right ? EdgeFlags.RightRemoved : EdgeFlags.LeftRemoved;
				}

				// 結果のポリゴン一覧に追加
				polygons.Add(edges);
			}

			return polygons;
		}

		/// <summary>
		/// 指定されたインデックスのポリゴンのみを抽出したポリゴンを作成する
		/// </summary>
		/// <param name="polygonIndex">抽出するポリゴンのインデックス</param>
		/// <returns>結果のポリゴンを構成するエッジと方向の一覧</returns>
		/// <remarks>事前に CreateTopology() を呼び出しておく必要がある。</remarks>
		public List<List<EdgeAndSide>> Extract(int polygonIndex) {
			BeginSetRemoveFlags();

			var polygons = new List<List<EdgeAndSide>>();

			// エッジの指定方向に指定ポリゴンが存在しないなら無視するフィルタ
			var edgeFilter = new Func<Edge, bool, bool>(
				(Edge e, bool right) => {
					List<int> r, l;
					if (right) {
						r = e.Right;
						l = e.Left;
					} else {
						l = e.Right;
						r = e.Left;
					}
					var rc = r.Contains(polygonIndex);
					var lc = l.Contains(polygonIndex);
					if (!rc)
						return true;
					if (lc)
						return true;
					return false;
				}
			);

			// 予め無視することがわかっているエッジを処理
			foreach (var edge in this.Edges) {
				if (edgeFilter(edge, true))
					edge.Flags |= EdgeFlags.RightRemoved;
				if (edgeFilter(edge, false))
					edge.Flags |= EdgeFlags.LeftRemoved;
			}

			// 右側にまだポリゴンが存在するエッジのみ処理する
			foreach (var edge in this.Edges) {
				if ((edge.Flags & EdgeFlags.RightRemoved) != 0)
					continue;

				// ポリゴンを構成するエッジと方向一覧を取得
				var edges = RightPolygon(edge, edgeFilter, true);

				// エッジの指定方向を処理済みとする
				foreach (var eas in edges) {
					eas.Edge.Flags |= eas.Right ? EdgeFlags.RightRemoved : EdgeFlags.LeftRemoved;
				}

				// 結果のポリゴン一覧に追加
				polygons.Add(edges);
			}

			return polygons;
		}
		#endregion

		#region 非公開メソッド
		/// <summary>
		/// 摘出フラグ設定の開始
		/// </summary>
		private void BeginSetRemoveFlags() {
			if(_RemoveFlagsIsSet) {
				foreach(var edge in this.Edges) {
					edge.Flags &= ~(EdgeFlags.RightRemoved | EdgeFlags.LeftRemoved);
				}
			}
			_RemoveFlagsIsSet = true;
		}

		/// <summary>
		/// 全エッジのうちポリゴンを構成できるところを全て構成する
		/// </summary>
		/// <returns>全てのポリゴンを構成するエッジと方向の一覧</returns>
		private List<List<EdgeAndSide>> PolygonizeAll() {
			var list = new List<List<EdgeAndSide>>();
			// 右側が未処理のエッジのみ処理する
			foreach (var edge in this.Edges) {
				if ((edge.Flags & EdgeFlags.RightPolygonized) != 0)
					continue;
				list.Add(PolygonizeRight(edge));
			}
			return list;
		}

		/// <summary>
		/// 指定されたエッジの右側を辿りポリゴンを構成するエッジ一覧を取得する
		/// </summary>
		/// <param name="edge">開始エッジ、このポリゴンの右側を辿る</param>
		/// <param name="edgeFilter">無視するエッジ判定用フィルタ、エッジと方向を受取無視するなら true を返す</param>
		/// <param name="mostOuter">true なら最も外側となるエッジを辿る、false なら最も内側となるエッジを辿る</param>
		/// <returns>ポリゴンを構成するエッジと方向の一覧</returns>
		private static List<EdgeAndSide> RightPolygon(Edge edge, Func<Edge, bool, bool> edgeFilter, bool mostOuter = false) {
			var list = new List<EdgeAndSide>();
			var startNode = edge.From; // ポリゴンの開始ノード
			var curNode = startNode;
			var nextNode = edge.To;
			var curIsRight = true; // エッジの右側を辿るなら true、左側なら false

			while (true) {
				// エッジのベクトルを計算
				var nextNodePos = nextNode.Position;
				var vec1 = curNode.Position - nextNodePos;
				var vec1v = vec1.VerticalCw();
				var vecLen1 = vec1.Length;

				if (mostOuter)
					vec1v = -vec1v;

				// 次のエッジを探す
				// 指定方向のノードに接続されたエッジで且つエッジ同士のなす右回りの角度が最も小さい又は大きいものを選ぶ
				var nextEdges = nextNode.Edges;
				var nextEdgesLen = nextEdges.Count;
				Edge nextEdge = null;
				Node nextNextNode = null;
				var nextIsRight = true;
				var maxCos = element.MinValue;

				for (int i = 0; i < nextEdgesLen; i++) {
					var e = nextEdges[i];
					if (e == edge)
						continue;
					var right = e.From == nextNode;
					if (edgeFilter(e, right))
						continue;
					var n2node = right ? e.To : e.From;
					var vec2 = n2node.Position - nextNodePos;
					var vecLen2 = vec2.Length;
					var cos = vec1.Dot(vec2) / (vecLen1 * vecLen2);
					if(0 <= vec1v.Dot(vec2)) {
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

				if (nextEdge == null) {
					throw new ApplicationException("エッジを辿りポリゴン生成中にヒゲを発見");
				}

				// ポリゴンを構成するエッジとして方向と共に登録
				list.Add(new EdgeAndSide(edge, curIsRight));

				if (nextNode == startNode)
					break;

				// 次回の準備
				edge = nextEdge;
				curNode = nextNode;
				nextNode = nextNextNode;
				curIsRight = nextIsRight;
			}

			return list;
		}

		/// <summary>
		/// 指定エッジの右側を辿りポリゴン化する
		/// </summary>
		/// <param name="edge">エッジ</param>
		/// <returns>ポリゴンを構成するエッジと方向の一覧</returns>
		private static List<EdgeAndSide> PolygonizeRight(Edge edge) {
			var list = RightPolygon(edge, (e, right) => false);
			var polygons = new HashSet<int>();

			// エッジの内側にあるポリゴン一覧を作成
			foreach (var eas in list) {
				foreach (var p in eas.Right ? eas.Edge.Right : eas.Edge.Left) {
					polygons.Add(p);
				}
			}

			// 内側にあるポリゴンをエッジ全体で統一し、処理済みとする
			foreach (var eas in list) {
				foreach (var p in polygons) {
					eas.Edge.LinkPolygon(eas.Right, p);
				}
				eas.Edge.Flags |= eas.Right ? EdgeFlags.RightPolygonized : EdgeFlags.LeftPolygonized;
			}

			return list;
		}

		/// <summary>
		/// AddPolygon() で追加されたポリゴンをノードとエッジに分解する
		/// </summary>
		private void MakeNodesAndEdges() {
			for(int polygonIndex = 0, n = _Polygons.Count; polygonIndex < n; polygonIndex++) {
				var pol = _Polygons[polygonIndex];

				// 頂点をノードに変換する、半径 _Epsilon を使いノードの接触を調べ、接触しているなら既存のノードを使用する
				var tpol = new TopologicalPolygon();
				foreach (var v in pol.Vertices) {
					var node = _NodeMgr.NewNode(v.Position);
					node.SetUserData(polygonIndex, v.UserData);
					tpol.Nodes.Add(node);
				}

				// ラインをエッジに変換する、既存エッジに同じノード組み合わせのものが存在したらそちらを使用する
				var nodes = tpol.Nodes;
				var nnodes = nodes.Count;
				var node1 = nodes[0];
				for (int i = 1; i <= nnodes; i++) {
					var node2 = nodes[i % nnodes];
					var edge = _EdgeMgr.NewEdge(node1, node2);
					edge.LinkPolygon(node1, polygonIndex);
					tpol.Edges.Add(edge);
					node1 = node2;
				}

				_TopologicalPolygons.Add(tpol);
			}
		}

		/// <summary>
		/// エッジとノードの接触、エッジ同士の交点部分にノードを挿入する
		/// </summary>
		private void MakeIntersectionNodes() {
			// 頂点とエッジの接触を調べ、ノード挿入情報を作成する
			for (int i = 0, n = _TopologicalPolygons.Count; i < n; i++) {
				InsertNodeToEdge(i);
			}

			// エッジ同士の交点を調べ、ノード挿入情報を作成する
			InsertIntersectionNodeToEdge();

			// 作成されたノード挿入情報を基にノードを挿入する
			EdgeDivide();
		}

		/// <summary>
		/// 指定ポリゴンのエッジに接触しているノードがあればエッジへ挿入予約する
		/// </summary>
		/// <param name="polygonIndex">エッジをチェックするポリゴンのインデックス</param>
		private void InsertNodeToEdge(int polygonIndex) {
			var polygon = _TopologicalPolygons[polygonIndex];
			var nodes = polygon.Nodes;
			var edges = polygon.Edges;
			var nedges = edges.Count;
			var epsilon2 = _Epsilon * _Epsilon;

			// 全エッジをチェックしていく
			for (int i = 0; i < nedges; i++) {
				// エッジから線分の情報取得
				var edge = edges[i];
				vector p, v;
				edge.GetLine(nodes[i], out p, out v);

				// エッジに接触する可能性があるノード探す
				foreach (var node in _NodeMgr.Query(edge.Volume)) {
					// ノードが未使用だったり指定されたポリゴンの一部ならスキップ
					if ((node.Flags & NodeFlags.Used) == 0)
						continue;
					if (node.IsLinked(polygonIndex))
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
					node.Flags |= NodeFlags.OnEdge;
					edge.SetNodeInsertion(t, node);
				}
			}
		}

		/// <summary>
		/// 全エッジの交差を調べ交差しているならエッジ上ノード挿入予約を行う
		/// </summary>
		private void InsertIntersectionNodeToEdge() {
			HashSet<ulong> comb = new HashSet<ulong>(); // 同じ組み合わせのチェックを排除するためのテーブル

			foreach (var edge1 in _EdgeMgr.AllEdges) {
				ulong uidx1 = edge1.UniqueIndex;

				// エッジから線分の情報取得
				vector p1, v1;
				edge1.GetLine(out p1, out v1);

				// エッジに接触する可能性があるエッジ探す
				foreach (var edge2 in _EdgeMgr.Query(edge1.Volume)) {
					ulong uidx2 = edge2.UniqueIndex;
					var combid = uidx1 <= uidx2 ? uidx2 << 32 | uidx1 : uidx1 << 32 | uidx2;

					// すでにチェック済みの組み合わせならスキップ
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

					// 交点座標のノードが既にエッジに挿入予約されていたらスキップ
					var node = _NodeMgr.NewNode(p1 + v1 * t1);
					if ((node.Flags & NodeFlags.OnEdge) != 0)
						continue;

					// このノードは内外が入れ替わるノード
					node.Flags |= NodeFlags.InsideOutside;

					// ノード挿入予約
					node.Flags |= NodeFlags.OnEdge;
					edge1.SetNodeInsertion(t1, node);
					edge2.SetNodeInsertion(t2, node);
				}
			}
		}

		/// <summary>
		/// エッジのノード挿入情報を基にノードを挿入する
		/// </summary>
		private void EdgeDivide() {
			// 現時点での全エッジを対象に処理する
			foreach (var edge in new List<Edge>(_EdgeMgr.AllEdges)) {
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
					var newEdge = _EdgeMgr.NewEdge(node1, ni.Node);

					newEdge.LinkPolygon(edge, node1 == newEdge.From);

					node1 = ni.Node;
				}

				var newEdge2 = _EdgeMgr.NewEdge(node1, edge.To);
				newEdge2.LinkPolygon(edge, node1 == newEdge2.From);

				// ノード挿入情報をクリア
				edge.NodeInsertions = null;

				// 元のエッジ削除
				_EdgeMgr.RemoveEdge(edge);
			}
		}

		/// <summary>
		/// 多角形の面積と回り方向を計算する
		/// </summary>
		/// <param name="vertices">[in] 多角形の頂点列</param>
		/// <returns>多角形の面積が返る、ポリゴンが反時計回りなら正数、時計回りなら負数となる</returns>
		public static element Area(vector[] vertices) {
			var nvts = vertices.Length;
			int i, n = nvts + 1;
			var p1 = vertices[0];
			element s = 0;
			for (i = 1; i < n; i++) {
				var p2 = vertices[i % nvts];
				s += (p1.X - p2.X) * (p1.Y + p2.Y);
				p1 = p2;
			}
			return s / 2;
		}

		/// <summary>
		/// pを開始点としvを方向ベクトルとする線分と点cとの最近点の線分パラメータを計算する
		/// </summary>
		/// <param name="p">[in] 線分の開始点</param>
		/// <param name="v">[in] 線分の方向ベクトル</param>
		/// <param name="c">[in] 最近点を調べる点c</param>
		/// <returns>線分のパラメータ</returns>
		public static element LinePointNearestParam(vector p, vector v, vector c) {
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
		public static bool LineIntersect(vector s1, vector e1, vector s2, vector e2) {
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
		public static bool Line2IntersectParam(vector p1, vector v1, vector p2, vector v2, out element t1, out element t2) {
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
		public static bool Line2IntersectParamCheckRange(vector p1, vector v1, vector p2, vector v2, element tolerance, out element t1, out element t2) {
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
		#endregion
	}
}
