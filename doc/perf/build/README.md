# Build Performance

The current working tree was compared against commit `3fb5d84562fdb49002078332dd4145b1e6afd70d` using `run/build-win.bat`.

- 1 unmeasured warm-up build per version
- 50 measured builds per version
- Versions alternated between measurements
- All 100 measured builds succeeded

| Metric | Current | Previous |
|---|---:|---:|
| Mean | 1.5043 s | 1.4172 s |
| Median | 1.3765 s | 1.3764 s |
| Minimum | 1.3501 s | 1.3439 s |
| Maximum | 4.6155 s | 2.1787 s |

Tukey's IQR rule detected five outliers in the current version and three in the previous version. Removing every paired round containing an outlier left 43 rounds:

| Metric | Current | Previous |
|---|---:|---:|
| Mean without outliers | 1.3824 s | 1.3782 s |
| Median without outliers | 1.3746 s | 1.3719 s |

The current version was about 0.30% slower after outlier removal, a difference of roughly 4.2 ms. The typical build time is effectively unchanged; the larger raw mean difference was caused by timing noise and outliers.
