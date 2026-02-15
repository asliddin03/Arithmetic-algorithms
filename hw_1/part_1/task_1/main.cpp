#include <iostream>
#include <iomanip>
#include <vector>

using std::cin;
using std::cout;
using std::vector;
using std::pair;

const long double EPS = 1e-12L;

bool isZero(long double val) {
    return fabsl(val) < EPS;
}

pair<bool, vector<long double> > gaussSolve(vector<vector<long double> > &G, vector<long double> &f) {
    int m = G.size();
    int n = (m ? G[0].size() : 0);

    vector<vector<long double> > G_f(m, vector<long double>(n + 1, 0));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            G_f[i][j] = G[i][j];
        }
        G_f[i][n] = f[i];
    }
    vector<int> where(n, -1);
    int row = 0;
    for (int col = 0; col < n && row < m; ++col) {
        int ind = -1;
        for (int i = row; i < m; i++) {
            if (!isZero(G_f[i][col])) {
                ind = i;
                break;
            }
        }
        if (ind == -1) {
            continue;
        }

        std::swap(G_f[row], G_f[ind]);
        where[col] = row;

        const long double pivot = G_f[row][col];
        for (int i = col; i <= n; ++i) {
            G_f[row][i] /= pivot;
        }

        for (int i  = 0; i < m; ++i) {
            if (i == row || isZero(G_f[i][col])) {
                continue;
            }

            for (int j = col + 1; j <= n; ++j) {
                G_f[i][j] -= G_f[i][col] * G_f[row][j];
            }
            G_f[i][col] = 0;
        }
        row++;
    }

    for (int i = 0; i < m; ++i) {
        bool all_zero = true;
        for (int j = 0; j < n; ++j) {
            if (!isZero(G_f[i][j])) {
                all_zero = false;
                break;
            }
        }
        if (all_zero && !isZero(G_f[i][n])) {
            return {false, {}};
        }
    }

    vector<long double> ans(n, 0);
    for (int col = 0; col < n; ++col) {
        if (where[col] != -1) {
            ans[col] = G_f[where[col]][n];
        } else {
            ans[col] = 0;
        }
    }

    return {true, ans};

}

int main() {
    int k;
    cin >> k;

    int deg_f;
    cin >> deg_f;
    vector<long double> func_f(deg_f + 2);
    for (int i = 0; i <= deg_f; ++i) {
        std::cin >> func_f[i];
    }

    vector<vector<long double> > func_g(k + 1);
    vector<int>deg_g(k);
    int size_ = deg_f + 1;
    for (int i = 0; i < k; ++i) {
        cin >> deg_g[i];
        func_g[i].resize(deg_g[i] + 1);
        for (int j = 0; j <= deg_g[i]; ++j) {
            cin >> func_g[i][j];
        }
        size_ = std::max(size_, deg_g[i] + 1);
    }

    func_f.resize(size_, 0.0);
    vector<vector<long double> > G(size_, vector<long double>(k, 0) );

    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < size_; ++j) {
            G[j][i] = func_g[i][j];
        }
    }

    auto[ok, A] = gaussSolve(G, func_f);

    if (!ok) {
        std::cout << "No solution" << std::endl;
    }

    cout.setf(std::ios::fixed);
    cout << std::setprecision(12);
    for (int i = 0; i < k; i++) {
        cout << A[i] << (i + 1 == k ? '\n' : ' ');
    }
}