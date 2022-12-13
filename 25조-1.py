from typing import *


def main():
    q: List[float] = [0.07, 0.12, 0.07, 0.07, 0.06, 0.05, 0.08, 0.06]  # fictitious node
    p: List[float] = [-1, 0.1, 0.09, 0.06, 0.05, 0.05, 0.04, 0.03]  # real node
    node_count: int = len(q)  # node 개수

    # 각 테이블 내부 공간 할당(초기화)
    weight_arr: List[List[float]] = [[0] * node_count for _ in range(node_count)]  # weight matrix
    cost_arr: List[List[float]] = [[0] * node_count for _ in range(node_count)]  # cost matrix
    root_arr: List[List[float]] = [[0] * node_count for _ in range(node_count)]  # root matrix

    for idx_diag in range(0, node_count):
        """
        cost 테이블, weight 테이블 초기화
        idx_diag: 테이블 초기화용 인덱싱 변수 (대각선 인덱싱)
        """
        weight_arr[idx_diag][idx_diag] = q[idx_diag]
        cost_arr[idx_diag][idx_diag] = 0

    for l_times in range(1, node_count):  # 알고리즘 상에서의 l이 대각선 순회 횟수이므로 l_times으로 명명
        # print(f"Debug>>> l값: {l_times}")
        for first_node in range(0, node_count - l_times):  # 알고리즘 상에서의 i가 판단의 첫 노드 번호이므로 first_node로 명명
            last_node: int = first_node + l_times  # 알고리즘 상에서의 j가 판단의 마지막 노드 번호이므로 last_node로 명명

            """
            가중치 계산 후 대입: 공식에 의해 특정 노드의 값 지정
            """
            weight_arr[first_node][last_node] = round(weight_arr[first_node][last_node-1] + p[last_node] + q[last_node], 2)
            cost_arr[first_node][last_node] = float('inf')  # 계산하려는 cost를 +inf로 초기화할 시 비교구문만으로 최소값 찾기 가능

            for root_temp in range(first_node + 1, last_node + 1):
                # print("-----")
                # print("현재 first_node: ", first_node)
                # print("현재 last_node: ", last_node)
                # print("현재 root_temp: ", root_temp)
                cost_temp: float = round(cost_arr[first_node][root_temp-1] + cost_arr[root_temp][last_node] + weight_arr[first_node][last_node], 2)  # 임시로 구해서 판단하는 cost값
                # print("현재 cost_temp: ", cost_temp)

                if cost_temp < cost_arr[first_node][last_node]:
                    """
                    기존 cost가 새 cost보다 클 경우(기존 cost가 inf인 경우를 포함)
                    새 cost를 저장하고
                    root 테이블에 나타나는 노드가 현재 k가 되어
                    노드를 나눈 노드 번호를 저장
                    """
                    # print("k값 변경됨")
                    cost_arr[first_node][last_node] = cost_temp
                    root_arr[first_node][last_node] = root_temp

    # 작업 완료된 테이블 출력
    print(">>>weight 테이블<<<")
    for print_idx in range(node_count):
        print(weight_arr[print_idx])

    print(">>>cost 테이블<<<")
    for print_idx in range(node_count):
        print(cost_arr[print_idx])

    print(">>>root 테이블<<<")
    for print_idx in range(node_count):
        print(root_arr[print_idx])


if __name__ == '__main__':
    main()
