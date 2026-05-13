# ECG_Simulate 仓库迁移说明

当前工程已迁移并收敛到 `ECG_Simulate/` 目录，可直接作为独立仓库根目录使用。

## 当前仓库结构

- `Core/Inc/`：头文件
- `Core/Src/`：源文件
- `README.md`：使用说明
- `.gitignore`：Keil 构建产物忽略规则

## 作为独立仓库使用

如果你需要将其变为单独 Git 仓库，可在 `ECG_Simulate/` 目录执行：

```bash
cd ECG_Simulate
git init
git add .
git commit -m "Initial import of ECG_Simulate project"
```

或从当前仓库拆分历史后推送到新的远端仓库。
