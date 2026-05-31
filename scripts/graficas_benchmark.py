#!/usr/bin/env python3
"""Genera gráficas comparativas a partir de datos/salida/benchmark_report.csv."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

ALGORITMOS = ["FuerzaBruta", "Voraz", "VorazMejorado", "ProgramacionDinamica"]
ETIQUETAS = {
    "FuerzaBruta": "Fuerza bruta",
    "Voraz": "Voraz",
    "VorazMejorado": "Voraz mejorado",
    "ProgramacionDinamica": "Prog. dinámica",
}
COLORES = {
    "FuerzaBruta": "#4C72B0",
    "Voraz": "#DD8452",
    "VorazMejorado": "#C44E52",
    "ProgramacionDinamica": "#55A868",
}


def etiqueta_finca(nombre: str) -> str:
    base = Path(nombre).stem.replace("finca_", "")
    return base.replace("_", " ")


def cargar_csv(ruta: Path) -> pd.DataFrame:
    df = pd.read_csv(ruta)
    numericas = [
        "N",
        "repeticiones",
        "tiempo_min_ms",
        "tiempo_mediana_ms",
        "tiempo_prom_ms",
        "tiempo_max_ms",
        "mem_min_kb",
        "mem_prom_kb",
        "mem_max_kb",
        "costo",
    ]
    for col in numericas:
        if col in df.columns:
            df[col] = pd.to_numeric(df[col], errors="coerce")
    df["omitido"] = df["tiempo_mediana_ms"].isna() & df["costo"].isna()
    df["finca_label"] = df["finca"].map(etiqueta_finca)
    return df


def pivot_ejecutados(df: pd.DataFrame, columna: str) -> pd.DataFrame:
    sub = df[~df["omitido"]].copy()
    orden_fincas = (
        sub.groupby("finca_label", as_index=False)["N"]
        .first()
        .sort_values("N")
        ["finca_label"]
        .tolist()
    )
    wide = sub.pivot(index="finca_label", columns="algoritmo", values=columna)
    wide = wide.reindex(index=orden_fincas, columns=ALGORITMOS)
    return wide


def grafico_barras(
    wide: pd.DataFrame,
    titulo: str,
    ylabel: str,
    salida: Path,
    logy: bool = False,
    umbral_log: float = 50.0,
) -> None:
    if wide.empty:
        return

    fincas = wide.index.tolist()
    x = np.arange(len(fincas))
    n_alg = len(ALGORITMOS)
    ancho = 0.8 / max(n_alg, 1)

    fig, ax = plt.subplots(figsize=(12, 6))
    for i, algo in enumerate(ALGORITMOS):
        if algo not in wide.columns:
            continue
        vals = wide[algo].values
        offset = (i - (n_alg - 1) / 2) * ancho
        ax.bar(
            x + offset,
            vals,
            width=ancho,
            label=ETIQUETAS[algo],
            color=COLORES[algo],
            edgecolor="white",
            linewidth=0.5,
        )

    usar_log = logy and wide.max().max() > umbral_log
    if usar_log:
        ax.set_yscale("log")
        ax.set_ylabel(f"{ylabel} (escala log)")
    else:
        ax.set_ylabel(ylabel)

    ax.set_title(titulo)
    ax.set_xticks(x)
    ax.set_xticklabels(fincas, rotation=35, ha="right")
    ax.legend(loc="upper left", framealpha=0.9)
    ax.grid(axis="y", alpha=0.3, which="both" if usar_log else "major")
    fig.tight_layout()
    fig.savefig(salida, dpi=150)
    plt.close(fig)


def grafico_tiempo_vs_n(df: pd.DataFrame, salida: Path) -> None:
    sub = df[~df["omitido"]].copy()
    if sub.empty:
        return

    fig, ax = plt.subplots(figsize=(10, 6))
    for algo in ALGORITMOS:
        parte = sub[sub["algoritmo"] == algo].sort_values("N")
        if parte.empty:
            continue
        ax.plot(
            parte["N"],
            parte["tiempo_mediana_ms"],
            marker="o",
            linewidth=2,
            label=ETIQUETAS[algo],
            color=COLORES[algo],
        )

    ax.set_yscale("log")
    ax.set_xlabel("N (tablones)")
    ax.set_ylabel("Tiempo mediana (ms, escala log)")
    ax.set_title("Tiempo de ejecución vs tamaño de finca")
    ax.legend()
    ax.grid(True, alpha=0.3, which="both")
    fig.tight_layout()
    fig.savefig(salida, dpi=150)
    plt.close(fig)


def grafico_costo(df: pd.DataFrame, salida: Path) -> None:
    wide = pivot_ejecutados(df, "costo")
    if wide.empty:
        return

    fincas = wide.index.tolist()
    x = np.arange(len(fincas))
    fig, ax = plt.subplots(figsize=(12, 6))

    for i, algo in enumerate(ALGORITMOS):
        if algo not in wide.columns:
            continue
        ax.plot(
            x,
            wide[algo].values,
            marker="s",
            linewidth=2,
            markersize=7,
            label=ETIQUETAS[algo],
            color=COLORES[algo],
        )

    ax.set_xticks(x)
    ax.set_xticklabels(fincas, rotation=35, ha="right")
    ax.set_ylabel("Costo total")
    ax.set_title("Costo de solución por finca y algoritmo")
    ax.legend()
    ax.grid(True, alpha=0.3)
    fig.tight_layout()
    fig.savefig(salida, dpi=150)
    plt.close(fig)


def grafico_ratio_optimo(df: pd.DataFrame, salida: Path) -> None:
    """Ratio costo / mejor costo disponible (FB o PD) por finca."""
    sub = df[~df["omitido"] & df["costo"].notna()].copy()
    filas = []
    for finca, grupo in sub.groupby("finca_label"):
        ref = grupo["costo"].min()
        if ref <= 0:
            continue
        for _, row in grupo.iterrows():
            filas.append(
                {
                    "finca_label": finca,
                    "N": row["N"],
                    "algoritmo": row["algoritmo"],
                    "ratio": row["costo"] / ref,
                }
            )
    if not filas:
        return

    ratio_df = pd.DataFrame(filas)
    wide = ratio_df.pivot(index="finca_label", columns="algoritmo", values="ratio")
    orden = (
        ratio_df.groupby("finca_label")["N"].first().sort_values().index.tolist()
    )
    wide = wide.reindex(index=orden, columns=ALGORITMOS)

    fincas = wide.index.tolist()
    x = np.arange(len(fincas))
    n_alg = len(ALGORITMOS)
    ancho = 0.8 / n_alg

    fig, ax = plt.subplots(figsize=(12, 6))
    for i, algo in enumerate(ALGORITMOS):
        if algo not in wide.columns:
            continue
        offset = (i - (n_alg - 1) / 2) * ancho
        ax.bar(
            x + offset,
            wide[algo].values,
            width=ancho,
            label=ETIQUETAS[algo],
            color=COLORES[algo],
            edgecolor="white",
            linewidth=0.5,
        )

    ax.axhline(1.0, color="#333", linestyle="--", linewidth=1, label="Óptimo (=1)")
    ax.set_xticks(x)
    ax.set_xticklabels(fincas, rotation=35, ha="right")
    ax.set_ylabel("Ratio respecto al mejor costo medido")
    ax.set_title("Calidad de solución (1 = óptimo entre algoritmos ejecutados)")
    ax.legend(loc="upper left")
    ax.grid(axis="y", alpha=0.3)
    fig.tight_layout()
    fig.savefig(salida, dpi=150)
    plt.close(fig)


def main() -> int:
    parser = argparse.ArgumentParser(description="Gráficas desde benchmark_report.csv")
    parser.add_argument(
        "--csv",
        type=Path,
        default=Path("datos/salida/benchmark_report.csv"),
    )
    parser.add_argument(
        "--out",
        type=Path,
        default=Path("datos/salida/graficas"),
    )
    args = parser.parse_args()

    if not args.csv.is_file():
        print(f"No existe el CSV: {args.csv}", file=sys.stderr)
        print("Ejecuta antes: make run-benchmark", file=sys.stderr)
        return 1

    args.out.mkdir(parents=True, exist_ok=True)
    df = cargar_csv(args.csv)

    plt.style.use("seaborn-v0_8-whitegrid")

    tiempo = pivot_ejecutados(df, "tiempo_mediana_ms")
    memoria = pivot_ejecutados(df, "mem_max_kb")

    grafico_barras(
        tiempo,
        "Tiempo de ejecución (mediana)",
        "Tiempo (ms)",
        args.out / "01_tiempo_mediana.png",
        logy=True,
    )
    grafico_barras(
        memoria,
        "Memoria pico (RSS máximo del proceso)",
        "Memoria (KiB)",
        args.out / "02_memoria_max.png",
        logy=True,
        umbral_log=5000,
    )
    grafico_tiempo_vs_n(df, args.out / "03_tiempo_vs_n.png")
    grafico_costo(df, args.out / "04_costo_por_finca.png")
    grafico_ratio_optimo(df, args.out / "05_ratio_optimo.png")

    print(f"Gráficas guardadas en: {args.out.resolve()}")
    for png in sorted(args.out.glob("*.png")):
        print(f"  - {png.name}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
