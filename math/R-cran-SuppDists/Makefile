# Created by: b.f. <bf@FreeBSD.org>
# $FreeBSD$

PORTNAME=	SuppDists
DISTVERSION=	1.1-9.1
PORTREVISION=	2
CATEGORIES=	math
MASTER_SITES=	CRAN \
		http://r.research.att.com/benchmarks/:benchmark \
		LOCAL/bf:benchmark
DISTFILES=	${PORTNAME}_${DISTVERSION}${EXTRACT_SUFX} \
		${BENCHMARK_SCRIPT}:benchmark
EXTRACT_ONLY=	${PORTNAME}_${DISTVERSION}${EXTRACT_SUFX}

MAINTAINER=	bf@FreeBSD.org
COMMENT=	Supplementary distributions and RNG for R

LICENSE=		GPLv2

BROKEN=		fails to build

BENCHMARK_SCRIPT=	R-benchmark-25.R
USES=	cran:auto-plist
WRKSRC=			${WRKDIR}/${PORTNAME}

.if defined(MAINTAINER_MODE)
USES+=	dos2unix
DOS2UNIX_FILES=	../${BENCHMARK_SCRIPT}

post-extract:
	@${CP} ${_DISTDIR}/${BENCHMARK_SCRIPT} ${WRKDIR}

post-install: benchmark

benchmark:
	@${LOCALBASE}/bin/Rscript --vanilla ${WRKDIR}/${BENCHMARK_SCRIPT}

.endif

.include <bsd.port.mk>
